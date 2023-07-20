#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "ndr_regex.h"

typedef struct NDR_RegexNode {
    bool start;
    bool end;
    bool optionalPath;
    bool repeatPath;
    bool orPath;
    bool wordStart;
    bool wordEnd;
    int minMatches;
    int maxMatches;

    struct NDR_RegexNode* wordReference;

    bool allButNewLine;
    bool everything;
    bool nothing;
    bool decimalDigit;
    bool notDecimalDigit;
    bool whiteSpace;
    bool notWhiteSpace;
    bool wordChar;
    bool notWordChar;

    size_t numberOfChars;
    size_t memoryAllocated;
    char* acceptChars;

    size_t numberOfChildren;
    size_t memoryAllocatedChildren;
    struct NDR_RegexNode** children;

} NDR_RegexNode;

typedef struct NDR_RegexTracker {
    NDR_RegexNode* reference;
    size_t numberOfRepeats;
    size_t currentChild;
    int stringPosition;
} NDR_RegexTracker;

typedef struct NDR_TrackerStack{
    NDR_RegexTracker** references;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_TrackerStack;

void ContinueAfterWord(NDR_RNodeStack* startStack, NDR_RNodeStack* endStack);
int HandleSpecialCharacters(NDR_RegexNode* node, char comp);
bool IsCharacterAccepted(NDR_RegexNode* node, char comp);
int checkAllButNewLine(char comp);
int checkEverything(char comp);
int checkDecimalDigit(char comp);
int checkNotDecimalDigit(char comp);
int checkWhiteSpace(char comp);
int checkNotWhiteSpace(char comp);
int checkWordChar(char comp);
int checkNotWordChar(char comp);

void NDR_InitRegex(NDR_Regex* cRegex);
void NDR_InitRegexNode(NDR_RegexNode* node);
bool NDR_IsRNodeEmpty(NDR_RegexNode* node);
bool NDR_IsRNodeSetForComparison(NDR_RegexNode* node);
void NDR_AddRNodeChar(NDR_RegexNode* node, char character);
void NDR_AddRNodeChild(NDR_RegexNode* node, NDR_RegexNode* child);
bool NDR_RNodeDuplicate(NDR_RegexNode* address, NDR_RegexNode** nodes, size_t index);

void NDR_InitRNodeStack(NDR_RNodeStack* ndrstack);
size_t NDR_RNodeStackSize(NDR_RNodeStack* ndrstack);
bool NDR_RNodeStackIsEmpty(NDR_RNodeStack* ndrstack);
void NDR_RNodeStackPush(NDR_RNodeStack* ndrstack, NDR_RegexNode* node);
NDR_RegexNode* NDR_RNodeStackPeek(NDR_RNodeStack* ndrstack);
NDR_RegexNode* NDR_RNodeStackPop(NDR_RNodeStack* ndrstack);
void NDR_RNodeStackSet(NDR_RNodeStack* ndrstack, NDR_RegexNode* node);

void NDR_InitRTracker(NDR_RegexTracker* ref, NDR_RegexNode* node);
void NDR_InitTrackerStack(NDR_TrackerStack* ndrstack);
size_t NDR_TrackerStackSize(NDR_TrackerStack* ndrstack);
bool NDR_TrackerStackIsEmpty(NDR_TrackerStack* ndrstack);
void NDR_TrackerStackPush(NDR_TrackerStack* ndrstack, NDR_RegexTracker* node);
NDR_RegexTracker* NDR_TrackerStackPeek(NDR_TrackerStack* ndrstack);
NDR_RegexTracker* NDR_TrackerStackPop(NDR_TrackerStack* ndrstack);


void NDR_DestroyRegexNode(NDR_RegexNode* node);
void NDR_DestroyRegexStack(NDR_RNodeStack* stack);
void NDR_DestroyRegexTracker(NDR_RegexTracker* tracker);
void NDR_DestroyRegexTrackerStack(NDR_TrackerStack* stack);



//Based on a provided regex string, create a reference graph for later matching the regex to other strings
int NDR_CompileRegex(NDR_Regex* cRegex, char* regexString){

    // Initialize needed values
    NDR_InitRegex(cRegex);

    // If the matching pattern is empty, set the flag and exit
    if(strcmp(regexString, "") == 0){
        cRegex->isEmpty = true;
        return 0;
    }


    // compilation state variables to understand the compilation time context
    char previousChar = '\0';
    bool isCurrentlyEscaped = false;
    bool startedCharClass = false;
    bool orJustSeen = false;

    // Initializing the stacks that will hold the regex nodes during parsing
    NDR_RNodeStack* startStack = malloc(sizeof(NDR_RNodeStack));
    NDR_InitRNodeStack(startStack);
    NDR_RNodeStack* endStack = malloc(sizeof(NDR_RNodeStack));
    NDR_InitRNodeStack(endStack);
    // Pushing the starting node into the start and end stack
    NDR_RegexNode* start1 = malloc(sizeof(NDR_RegexNode));
    NDR_InitRegexNode(start1);
    NDR_RNodeStackPush(startStack, start1);
    NDR_RNodeStackPush(endStack, start1);

    // loop through each character in the regex string
    for(size_t x = 0; x < strlen(regexString); x++){

        // If the escape character is found, set the state variables accordingly and go to the next iteration
        if(regexString[x] == '\\' && isCurrentlyEscaped == false){
            isCurrentlyEscaped = true;
            previousChar = '\\';
            continue;
        }

        // if the first char in string is the begin string anchor
        if(x == 0 && regexString[x] == '$'){
            cRegex->beginString = true;
        }
        // if the last char in string is the end string anchor and it is not escaped
        else if(x == strlen(regexString) - 1 && isCurrentlyEscaped == false && regexString[x] == '%'){
            cRegex->endString = true;
        }
        // Perform look ahead for use of the or operator '|' without a word following it
        else if(orJustSeen == true && regexString[x] != '(' ){
            printf("Invalid use of '|' operator in regex at char %i. A parentheses enclosed \"word\" must follow the '|' symbol", x);
            return -1;
        }
        //Otherwise handle all characters
        else{
            // When the special character '(' is seen, it is assumed to be the beginning of a new "word"
            // Place a new item on the stack to isolate the new word and set the path in the graph
            if(regexString[x] == '(' && isCurrentlyEscaped == false && startedCharClass == false){
                NDR_RegexNode* newNode = malloc(sizeof(NDR_RegexNode));
                NDR_InitRegexNode(newNode);
                NDR_RNodeStackPush(startStack, newNode);
                NDR_RNodeStackPeek(startStack)->wordStart = true;
                // if the or operator '|' was just seen set the beginning of the graph path to or for retroactive updating
                if(orJustSeen == true){
                    NDR_RNodeStackPeek(startStack)->orPath = true;
                }
                NDR_RNodeStackPush(endStack, NDR_RNodeStackPeek(startStack)->children[0]);
                NDR_InitRegexNode(NDR_RNodeStackPeek(endStack));
            }
            // When the special character ')' is seen, it is assumed to be the end of a "word"
            else if(regexString[x] == ')' && isCurrentlyEscaped == false && startedCharClass == false){
                if(NDR_IsRNodeEmpty(NDR_RNodeStackPeek(endStack)) == false){
                    NDR_InitRegexNode(NDR_RNodeStackPeek(endStack)->children[0]);
                    NDR_RNodeStackSet(endStack, NDR_RNodeStackPeek(endStack)->children[0]);
                }
                NDR_RNodeStackPeek(endStack)->wordEnd = true;
                NDR_RNodeStackPeek(endStack)->wordReference = NDR_RNodeStackPeek(startStack);


                if(NDR_RNodeStackPeek(startStack)->orPath == true){
                    NDR_RNodeStackPeek(endStack)->orPath = true;

                    NDR_RegexNode* holdStart = NDR_RNodeStackPop(startStack);
                    NDR_RegexNode* holdEnd = NDR_RNodeStackPop(endStack);
                    NDR_RegexNode* follow = NDR_RNodeStackPeek(startStack);
                    while(follow != NDR_RNodeStackPeek(endStack)){

                        if(follow->orPath == true){
                            NDR_AddRNodeChild(follow, holdStart->children[0]);
                            // Need to free holdstart itself in this case
                            follow = holdStart->children[0];
                            while(follow->children[0] != holdEnd){
                                follow = follow->children[0];
                            }
                            follow->children[0] = NDR_RNodeStackPeek(endStack);
                            break;
                        }
                        else{
                            follow = follow->children[0];
                        }
                    }

                    if(regexString[x+1] == '|'){
                        orJustSeen = true;
                        x++;
                    }
                }
                else if(strlen(regexString) > x+1 && regexString[x+1] == '|'){

                    if(NDR_RNodeStackPeek(startStack)->orPath == false){
                        NDR_RNodeStackPeek(startStack)->orPath = true;
                        NDR_RNodeStackPeek(endStack)->orPath = true;
                        NDR_RegexNode* holdStart = NDR_RNodeStackPop(startStack);
                        NDR_RegexNode* holdEnd = NDR_RNodeStackPop(endStack);
                        NDR_RegexNode* follow = NDR_RNodeStackPeek(startStack);
                        while(follow != NDR_RNodeStackPeek(endStack)){
                            if(follow->orPath == true){
                                NDR_AddRNodeChild(follow, holdStart->children[0]);
                                // Need to free holdstart itself in this case
                                follow = holdStart->children[0];
                                while(follow->children[0] != holdEnd){
                                    follow = follow->children[0];
                                }
                                follow->children[0] = NDR_RNodeStackPeek(endStack);
                                break;
                            }
                            else{
                                follow = follow->children[0];
                            }
                        }
                        NDR_RNodeStackPeek(endStack)->children[0] = holdStart;
                        NDR_RNodeStackSet(endStack, holdEnd);

                    }

                    orJustSeen = true;

                    x++;
                }
                else if(strlen(regexString) > x+1){

                    if(regexString[x+1] == '*'){
                        NDR_RNodeStackPeek(startStack)->repeatPath = true;
                        NDR_RNodeStackPeek(endStack)->repeatPath = true;
                        NDR_RNodeStackPeek(startStack)->optionalPath = true;
                        NDR_RNodeStackPeek(endStack)->optionalPath = true;
                        NDR_RNodeStackPeek(startStack)->minMatches = 0;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 0;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        //ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '+'){
                        NDR_RNodeStackPeek(startStack)->repeatPath = true;
                        NDR_RNodeStackPeek(endStack)->repeatPath = true;
                        NDR_RNodeStackPeek(startStack)->minMatches = 1;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 1;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        //ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '?'){
                        NDR_RNodeStackPeek(startStack)->optionalPath = true;
                        NDR_RNodeStackPeek(endStack)->optionalPath = true;
                        //ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '{'){
                        x++;
                        if(strlen(regexString) <= x+3){
                            printf("Invalid numerator in regex at char %i", x);
                            return -1;
                        }
                        // Initialize the repeat numbers
                        char* repeatNum1 = malloc((strlen(regexString) - x) + 2);
                        char* repeatNum2 = malloc((strlen(regexString) - x) + 2);
                        strcpy(repeatNum1, "1");
                        strcpy(repeatNum2, "1");
                        bool isCommaPresent = false;
                        int jump = 0;
                        for(int i = x+1; i < strlen(regexString) && regexString[i] != '}'; i++){
                            if(isdigit(regexString[i]) > 0 && isCommaPresent == false){
                                repeatNum1[i - (x+1)] = regexString[i];
                                repeatNum1[(i - (x+1)) + 1] = '\0';
                            }
                            else if(regexString[i] == ','){
                                isCommaPresent = true;
                            }
                            else if(isdigit(regexString[i]) > 0 && isCommaPresent == true){
                                repeatNum2[i - ((x+1) + strlen(repeatNum1) + 1)] = regexString[i];
                                repeatNum2[(i - ((x+1) + strlen(repeatNum1) + 1)) + 1] = '\0';
                            }
                            else{
                                printf("Invalid numerator in regex at char %i. Numerators should either contain one number or two numbers separated by a comma %c", x, regexString[x]);
                                return -1;
                            }
                            jump = i;
                        }
                        x = jump+1;

                        NDR_RNodeStackPeek(startStack)->repeatPath = true;
                        NDR_RNodeStackPeek(endStack)->repeatPath = true;
                        NDR_RNodeStackPeek(startStack)->minMatches = atoi(repeatNum1);
                        NDR_RNodeStackPeek(endStack)->minMatches = atoi(repeatNum1);
                        if(isCommaPresent == false){
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum1);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum1);
                        }
                        else{
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum2);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum2);
                        }

                        //ContinueAfterWord(startStack, endStack);
                    }

                    ContinueAfterWord(startStack, endStack);

                }
                else{
                    ContinueAfterWord(startStack, endStack);
                }

                continue;
            }
            else if(regexString[x] == '[' && isCurrentlyEscaped == false){

                if(startedCharClass == false){
                    NDR_RegexNode* newNode = malloc(sizeof(NDR_RegexNode));
                    NDR_InitRegexNode(newNode);
                    NDR_RNodeStackPush(startStack, newNode);
                    NDR_RNodeStackPush(endStack, NDR_RNodeStackPeek(startStack));

                    startedCharClass = true;
                }
                else if(startedCharClass == true){
                    printf("Invalid character class in regex at char %i", x+1);
                    return -1;
                }
            }
            else if(regexString[x] == ']' && isCurrentlyEscaped == false){
                if(startedCharClass == false){
                    printf("Invalid character class in regex at char %i", x+1);
                    return -1;
                }
                else if(startedCharClass == true){

                    if(strlen(regexString) > x+1){
                        if(regexString[x+1] == '*'){
                            NDR_RNodeStackPeek(startStack)->minMatches = 0;
                            NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                            NDR_RNodeStackPeek(endStack)->minMatches = 0;
                            NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                            //ContinueAfterWord(startStack, endStack);
                            x++;
                        }
                        else if(regexString[x+1] == '+'){

                            NDR_RNodeStackPeek(startStack)->minMatches = 1;
                            NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                            NDR_RNodeStackPeek(endStack)->minMatches = 1;
                            NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                            //ContinueAfterWord(startStack, endStack);
                            x++;
                        }
                        else if(regexString[x+1] == '?'){

                            NDR_RNodeStackPeek(endStack)->minMatches = 0;
                            NDR_RNodeStackPeek(endStack)->maxMatches = 1;
                            //ContinueAfterWord(startStack, endStack);
                            x++;
                        }
                        else if(regexString[x+1] == '{'){
                            x++;
                            if(strlen(regexString) <= x+3){
                                printf("Invalid numerator in regex at char %i", x+1);
                                return -1;
                            }
                            // trying to repeat a nonexistent character is invalid
                            else if(NDR_RNodeStackPeek(endStack)->numberOfChars == 0){
                                printf("Invalid numerator in regex at char %i", x+1);
                                return -1;
                            }
                            // Initialize the repeat numbers
                            char* repeatNum1 = malloc((strlen(regexString) - x) + 2);
                            char* repeatNum2 = malloc((strlen(regexString) - x) + 2);
                            strcpy(repeatNum1, "1");
                            strcpy(repeatNum2, "1");
                            bool isCommaPresent = false;
                            int jump = 0;
                            for(int i = x+1; i < strlen(regexString) && regexString[i] != '}'; i++){
                                if(isdigit(regexString[i]) > 0 && isCommaPresent == false){
                                    repeatNum1[i - (x+1)] = regexString[i];
                                    repeatNum1[(i - (x+1)) + 1] = '\0';
                                }
                                else if(regexString[i] == ','){
                                    isCommaPresent = true;
                                }
                                else if(isdigit(regexString[i]) > 0 && isCommaPresent == true){
                                    repeatNum2[i - ((x+1) + strlen(repeatNum1) + 1)] = regexString[i];
                                    repeatNum2[(i - ((x+1) + strlen(repeatNum1) + 1)) + 1] = '\0';
                                }
                                else{
                                    printf("Invalid numerator in regex at char %i. Numerators should either contain one number or two numbers separated by a comma %c", x, regexString[x]);
                                    return -1;
                                }
                                jump = i;
                            }
                            x = jump+1;


                            NDR_RNodeStackPeek(startStack)->minMatches = atoi(repeatNum1);
                            NDR_RNodeStackPeek(endStack)->minMatches = atoi(repeatNum1);
                            if(isCommaPresent == false){
                                NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum1);
                                NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum1);
                            }
                            else{
                                NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum2);
                                NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum2);
                            }

                        }
                        //else{
                        //    ContinueAfterWord(startStack, endStack);
                        //}
                    }
                    //else{
                    //    ContinueAfterWord(startStack, endStack);
                    //}
                    ContinueAfterWord(startStack, endStack);

                    startedCharClass = false;
                }
            }
            else if(regexString[x] == '{' && isCurrentlyEscaped == false && startedCharClass == false){
                printf("Invalid '{' operator at char %i", x+1);
                return -1;
            }
            else if(regexString[x] == '}' && isCurrentlyEscaped == false && startedCharClass == false){
                printf("Invalid numerator closing '}' at char %i", x+1);
                return -1;
            }
            else if(regexString[x] == '?' && isCurrentlyEscaped == false && startedCharClass == false){
                printf("Invalid '?' operator at char %i", x+1);
                return -1;
            }
            else if(regexString[x] == '*' && isCurrentlyEscaped == false && startedCharClass == false){
                printf("Invalid '*' operator at char %i", x+1);
                return -1;
            }
            else if(regexString[x] == '+' && isCurrentlyEscaped == false && startedCharClass == false){
                printf("Invalid '+' operator at char %i", x+1);
                return -1;
            }
            else if(regexString[x] == '|' && isCurrentlyEscaped == false && startedCharClass == false){
                printf("The '|' \"or\" operator must be used after a parentheses enclosed \"word\" or it must be escaped for literal use");
                return -1;
            }
            else if(startedCharClass == true){
                if(isCurrentlyEscaped == true){
                    if(regexString[x] == '-'){
                        NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), regexString[x]);
                    }
                    else
                        HandleSpecialCharacters(NDR_RNodeStackPeek(endStack), regexString[x]);
                }
                else{
                    if(regexString[x] == '-'){
                        if(x != strlen(regexString) - 1){
                            int difference = (int)previousChar - (int)regexString[x+1];
                            if(difference < 0)
                                difference = difference * -1;

                            int smallest = 0;
                            if(previousChar < regexString[x+1])
                                smallest = (int) previousChar;
                            else
                                smallest = (int) regexString[x+1];

                            for(int ch = smallest; ch <= smallest + difference; ch++){
                                NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), (char) ch);
                            }
                            x++;
                        }
                        else{
                            printf("invalid regex");
                            return -1;
                        }

                    }
                    else if(regexString[x] == '.'){
                        NDR_RNodeStackPeek(endStack)->everything = true;
                    }
                    else
                        NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), regexString[x]);
                }
            }
            else{// Normal character not in char class

                if(NDR_RNodeStackPeek(endStack)->numberOfChars != 0 || NDR_RNodeStackPeek(endStack)->wordEnd == true){
                    NDR_RNodeStackSet(endStack, NDR_RNodeStackPeek(endStack)->children[0]);
                    NDR_InitRegexNode(NDR_RNodeStackPeek(endStack));
                }

                if(isCurrentlyEscaped == true){
                    HandleSpecialCharacters(NDR_RNodeStackPeek(endStack), regexString[x]);
                }
                else if(regexString[x] == '.'){
                    NDR_RNodeStackPeek(endStack)->everything = true;
                }
                else{
                    NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), regexString[x]);
                }

                if(strlen(regexString) > x+1){

                    if(regexString[x+1] == '*'){

                        NDR_RNodeStackPeek(startStack)->minMatches = 0;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 0;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '+'){

                        NDR_RNodeStackPeek(startStack)->minMatches = 1;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 1;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '?'){

                        NDR_RNodeStackPeek(endStack)->minMatches = 0;
                        NDR_RNodeStackPeek(endStack)->maxMatches = 1;
                        ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '{'){
                        x++;
                        if(strlen(regexString) <= x+3){
                            printf("Invalid numerator in regex at char %i", x+1);
                            return -1;
                        }
                        // trying to repeat a nonexistent character is invalid
                        else if(NDR_RNodeStackPeek(endStack)->numberOfChars == 0){
                            printf("Invalid numerator in regex at char %i", x+1);
                            return -1;
                        }
                        // Initialize the repeat numbers
                        char* repeatNum1 = malloc((strlen(regexString) - x) + 2);
                        char* repeatNum2 = malloc((strlen(regexString) - x) + 2);
                        strcpy(repeatNum1, "1");
                        strcpy(repeatNum2, "1");
                        bool isCommaPresent = false;
                        int jump = 0;
                        for(int i = x+1; i < strlen(regexString) && regexString[i] != '}'; i++){
                            if(isdigit(regexString[i]) > 0 && isCommaPresent == false){
                                repeatNum1[i - (x+1)] = regexString[i];
                                repeatNum1[(i - (x+1)) + 1] = '\0';
                            }
                            else if(regexString[i] == ','){
                                isCommaPresent = true;
                            }
                            else if(isdigit(regexString[i]) > 0 && isCommaPresent == true){
                                repeatNum2[i - ((x+1) + strlen(repeatNum1) + 1)] = regexString[i];
                                repeatNum2[(i - ((x+1) + strlen(repeatNum1) + 1)) + 1] = '\0';
                            }
                            else{
                                printf("Invalid numerator in regex at char %i. Numerators should either contain one number or two numbers separated by a comma %c", x, regexString[x]);
                                return -1;
                            }
                            jump = i;
                        }
                        x = jump+1;


                        NDR_RNodeStackPeek(startStack)->minMatches = atoi(repeatNum1);
                        NDR_RNodeStackPeek(endStack)->minMatches = atoi(repeatNum1);
                        if(isCommaPresent == false){
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum1);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum1);
                        }
                        else{
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum2);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum2);
                        }
                        ContinueAfterWord(startStack, endStack);
                    }
                }


            }

        }


        isCurrentlyEscaped = false;
        previousChar = regexString[x];
        orJustSeen = false;

    }

    NDR_RegexNode* graphGetter;
    if(NDR_RNodeStackIsEmpty(endStack) == false){
        NDR_InitRegexNode(NDR_RNodeStackPeek(endStack)->children[0]);
        NDR_RNodeStackPeek(endStack)->children[0]->end = true;
    }

    while(NDR_RNodeStackIsEmpty(startStack) == false){
        graphGetter = NDR_RNodeStackPop(startStack);
        NDR_RNodeStackPop(endStack);
        if(NDR_RNodeStackIsEmpty(endStack) == false){
            NDR_RNodeStackPeek(endStack)->children[0] = graphGetter;
        }
    }

    cRegex->start->children[0] = graphGetter;
    NDR_RegexNode* follow = cRegex->start->children[0];
    while(follow->end != true){
        follow = follow->children[0];
    }

    NDR_DestroyRegexStack(startStack);
    NDR_DestroyRegexStack(endStack);
    free(startStack);
    free(endStack);


    return 0;

 }


/// Compare a string to a pre-compiled regex graph
NDR_MatchResult NDR_MatchRegex(NDR_Regex* cRegex, char* token){

    // Compare the NDR_CharDescriptor** parts of NDR_Regex type to each consecutive character within the token string

    if(strcmp(token, "") == 0 && cRegex->isEmpty == true){
        return COMPLETEMATCH;
    }
    else if(strcmp(token, "") == 0 || cRegex->isEmpty == true){
        return NOMATCH;
    }

    // Setting match state variables for tracking the state during matching
    NDR_MatchResult result = NOMATCH;
    int currentIndex = 0;
    int numTimesMatched = 0;

    // Setting up the stack for keeping track of all word paths within the graph
    NDR_RegexNode* follow = cRegex->start->children[0];
    NDR_TrackerStack* wordReferences = malloc(sizeof(NDR_TrackerStack));
    NDR_InitTrackerStack(wordReferences);

    for(int i = 0; i < strlen(token); i++){

        if(follow->end == true && cRegex->endString == true){
            return NOMATCH;
        }

        while(follow->end != true){

            //printf("ERTSHSYRJN %i\n", follow->children[0]->end == true);
            if(follow->wordStart == true){
                //printf("Aegerhg %c\n", follow->children[0]->children[0]->children[0]->acceptChars[0]);

                NDR_RegexTracker* ref = malloc(sizeof(NDR_RegexTracker));
                NDR_InitRTracker(ref, follow);
                NDR_TrackerStackPush(wordReferences, ref);
                NDR_TrackerStackPeek(wordReferences)->stringPosition = i;
                follow = follow->children[0];

                continue;
            }
            else if(follow->wordEnd == true){

                if(NDR_TrackerStackPeek(wordReferences)->reference->repeatPath == true &&
                   NDR_TrackerStackPeek(wordReferences)->reference->maxMatches > NDR_TrackerStackPeek(wordReferences)->numberOfRepeats){

                    (NDR_TrackerStackPeek(wordReferences)->numberOfRepeats)++;
                    follow = NDR_TrackerStackPeek(wordReferences)->reference;
                    NDR_TrackerStackPeek(wordReferences)->stringPosition = i;
                }
                else{
                    NDR_TrackerStackPop(wordReferences);
                }

                follow = follow->children[0];

                continue;

            }

            //printf("trying %c %c %i %i\n", token[i], follow->acceptChars[x], follow->minMatches, follow->maxMatches);
            if(IsCharacterAccepted(follow, token[i]) == true){
                //printf("matched %c %c\n", token[i], follow->acceptChars[x]);
                numTimesMatched++;
                result = PARTIALMATCH;

                if(numTimesMatched >= follow->minMatches){//printf("RYNJ7\n");
                    if(numTimesMatched >= follow->maxMatches && follow->maxMatches != -1){
                        follow = follow->children[0];            //printf("ERTSHSYRJN %i\n", follow->wordEnd == true);
                        numTimesMatched = 0;
                    }

                    if(NDR_TrackerStackIsEmpty(wordReferences) == false){
                        if(follow->wordEnd == true && follow->children[0]->end == true){
                            continue;
                        }
                    }

                    break;
                }
                else if(numTimesMatched < follow->minMatches){//printf("RYNJ8\n");
                    i++;
                }


            }
            else{
                // Currently only handling anchored regex
                //printf("RYNDTYM %c %c\n", token[i], follow->acceptChars[0]);
                bool isWord = false;
                while(NDR_TrackerStackIsEmpty(wordReferences) == false){
                    isWord = true;
                    if(NDR_TrackerStackPeek(wordReferences)->reference->optionalPath == true){//printf("RSY1\n");
                        i = NDR_TrackerStackPeek(wordReferences)->stringPosition;
                        NDR_RegexTracker* holdRef = NDR_TrackerStackPop(wordReferences);
                        // The if and else block will allow for an optional path in an or path to continue through the other or options
                        //if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                            while(follow->wordReference != holdRef->reference){
                                follow = follow->children[0];
                            }
                            follow = follow->children[0];
                        //}

                        /*else{
                            continue;
                        }*/
                    }
                    else if(NDR_TrackerStackPeek(wordReferences)->reference->repeatPath == true){//printf("RSY2\n");

                        if(NDR_TrackerStackPeek(wordReferences)->reference->minMatches > NDR_TrackerStackPeek(wordReferences)->numberOfRepeats && cRegex->beginString == true){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true)
                                return NOMATCH;
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->minMatches > NDR_TrackerStackPeek(wordReferences)->numberOfRepeats){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                i = currentIndex++;
                                follow = cRegex->start->children[0];
                                numTimesMatched = 0;
                            }
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->minMatches <= NDR_TrackerStackPeek(wordReferences)->numberOfRepeats){
                            i = NDR_TrackerStackPeek(wordReferences)->stringPosition;

                            while(follow->wordReference != NDR_TrackerStackPeek(wordReferences)->reference){
                                follow = follow->children[0];
                            }
                            follow = follow->children[0];
                            numTimesMatched = 0;
                            NDR_TrackerStackPop(wordReferences);
                        }

                    }
                    else if(NDR_TrackerStackPeek(wordReferences)->reference->orPath == true){//printf("RSY3\n");
                        if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 <= NDR_TrackerStackPeek(wordReferences)->currentChild && cRegex->beginString == true){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true)
                                return NOMATCH;
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 <= NDR_TrackerStackPeek(wordReferences)->currentChild){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                i = currentIndex++;
                                follow = cRegex->start->children[0];
                                numTimesMatched = 0;
                            }
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 > NDR_TrackerStackPeek(wordReferences)->currentChild){
                            i = NDR_TrackerStackPeek(wordReferences)->stringPosition;
                            follow = NDR_TrackerStackPeek(wordReferences)->reference->children[++(NDR_TrackerStackPeek(wordReferences)->currentChild)];
                            numTimesMatched = 0;
                        }

                    }
                    else{//printf("RSY4\n");
                        if(cRegex->beginString == true){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true)
                                return NOMATCH;
                            else
                                continue;
                        }
                        else{
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                i = currentIndex++;
                                follow = cRegex->start->children[0];
                                numTimesMatched = 0;
                            }
                            else
                                continue;
                        }
                    }
                    break;
                }
                if(isWord == true)
                    continue;

                if(follow->minMatches > numTimesMatched && cRegex->beginString == true){
                    //printf("RYNJ5\n");
                    return NOMATCH;
                }
                else if(follow->minMatches > numTimesMatched){
                    //printf("RYNJ6\n");
                    i = currentIndex++;
                    follow = cRegex->start->children[0];
                    numTimesMatched = 0;
                    while(NDR_TrackerStackIsEmpty(wordReferences) == false){
                        NDR_TrackerStackPop(wordReferences);
                    }
                    result = NOMATCH;
                    //break;
                }
                else if(follow->minMatches <= numTimesMatched){// If matching and there is a failure to match after the min number of matches has been met then proceed to the next node and restart
                    //printf("RYNJ3\n");
                    numTimesMatched = 0;
                    follow = follow->children[0];
                    i--;
                    //break;
                }

                break;

            }



        }

    }

    if(follow->end == false){
        return result;
    }

    NDR_DestroyRegexTrackerStack(wordReferences);
    free(wordReferences);

    return COMPLETEMATCH;
}

void ContinueAfterWord(NDR_RNodeStack* startStack, NDR_RNodeStack* endStack){
    /// Will need this commented code
    NDR_RegexNode* holdStart = NDR_RNodeStackPop(startStack);
    NDR_RegexNode* holdEnd = NDR_RNodeStackPop(endStack);

    NDR_RegexNode* follow = NDR_RNodeStackPeek(startStack);

    if(NDR_RNodeStackPeek(startStack) == NDR_RNodeStackPeek(endStack) && NDR_IsRNodeEmpty(NDR_RNodeStackPeek(endStack)) == true){

        NDR_RNodeStackSet(startStack, holdStart);
        NDR_RNodeStackSet(endStack, holdEnd);

    }
    else if(NDR_RNodeStackPeek(startStack) == NDR_RNodeStackPeek(endStack)){

        NDR_RNodeStackPeek(endStack)->children[0] = holdStart;
        NDR_RNodeStackSet(endStack, holdEnd);
    }
    else{
        if(NDR_IsRNodeEmpty(NDR_RNodeStackPeek(endStack)) == false){

            NDR_RNodeStackPeek(endStack)->children[0] = holdStart;
            NDR_InitRegexNode(holdEnd->children[0]);
            NDR_RNodeStackSet(endStack, holdEnd);
        }
        else{

            while(follow->children[0] != NDR_RNodeStackPeek(endStack)){
                follow = follow->children[0];
            }

            follow->children[0] = holdStart;
            NDR_RNodeStackSet(endStack, holdEnd);
        }

    }

}


int HandleSpecialCharacters(NDR_RegexNode* node, char comp){

    // Below are the special characters for denoting specific classes of characters
    if(comp == 'N'){
        node->allButNewLine = true;
    }
    else if(comp == 'e'){
        node->everything = true;
    }
    else if(comp == 'E'){
        node->nothing = true;
    }
    else if(comp == 'd'){
        node->decimalDigit = true;
    }
    else if(comp == 'D'){
        node->notDecimalDigit = true;
    }
    else if(comp == 's'){
        node->whiteSpace = true;
    }
    else if(comp == 'S'){
        node->notWhiteSpace = true;
    }
    else if(comp == 'w'){
        node->wordChar = true;
    }
    else if(comp == 'W'){
        node->notWordChar = true;
    }
    // Below are the special characters for structural specification within the regex engine
    else if(comp == '['){
        NDR_AddRNodeChar(node, '[');
    }
    else if(comp == ']'){
        NDR_AddRNodeChar(node, ']');
    }
    else if(comp == '('){
        NDR_AddRNodeChar(node, '(');
    }
    else if(comp == ')'){
        NDR_AddRNodeChar(node, ')');
    }
    else if(comp == '{'){
        NDR_AddRNodeChar(node, '{');
    }
    else if(comp == '}'){
        NDR_AddRNodeChar(node, '}');
    }
    else if(comp == '|'){
        NDR_AddRNodeChar(node, '|');
    }
    else if(comp == '$'){
        NDR_AddRNodeChar(node, '$');
    }
    else if(comp == '%'){
        NDR_AddRNodeChar(node, '%');
    }
    else if(comp == '?'){
        NDR_AddRNodeChar(node, '?');
    }
    else if(comp == '*'){
        NDR_AddRNodeChar(node, '*');
    }
    else if(comp == '+'){
        NDR_AddRNodeChar(node, '+');
    }
    else if(comp == '.'){
        NDR_AddRNodeChar(node, '.');
    }
    // Below are the special characters within the c language
    else if(comp == 'a'){
        NDR_AddRNodeChar(node, '\a');
    }
    else if(comp == 'b'){
        NDR_AddRNodeChar(node, '\b');
    }
    else if(comp == 'f'){
        NDR_AddRNodeChar(node, '\f');
    }
    else if(comp == 'n'){
        NDR_AddRNodeChar(node, '\n');
    }
    else if(comp == 'r'){
        NDR_AddRNodeChar(node, '\r');
    }
    else if(comp == 't'){
        NDR_AddRNodeChar(node, '\t');
    }
    else if(comp == 'v'){
        NDR_AddRNodeChar(node, '\v');
    }
    else if(comp == '\\'){
        NDR_AddRNodeChar(node, '\\');
    }
    else{
        return -1;
    }

    return 0;
}

bool IsCharacterAccepted(NDR_RegexNode* node, char comp){

    bool validChar = false;

    for(int x = 0; x < node->numberOfChars; x++){
        //printf("trying %c %c %i %i\n", comp, node->acceptChars[x], node->minMatches, node->maxMatches);
        if(comp == node->acceptChars[x]){
            //printf("matched %c %c\n", comp, node->acceptChars[x]);
            validChar = true;
            break;
        }
    }

    if(node->allButNewLine == true){//printf("trying allButNewLine %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkAllButNewLine(comp) == 0){//printf("matched allButNewLine %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->everything == true){//printf("trying everything %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkEverything(comp) == 0){//printf("matched everything %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->decimalDigit == true){//printf("trying decimalDigit %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkDecimalDigit(comp) == 0){//printf("matched decimalDigit %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
   if(node->notDecimalDigit == true){//printf("trying notDecimalDigit %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkNotDecimalDigit(comp) == 0){//printf("matched notDecimalDigit %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->whiteSpace == true){//printf("trying whiteSpace %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkWhiteSpace(comp) == 0){//printf("matched whiteSpace %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->notWhiteSpace == true){//printf("trying notWhiteSpace %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkNotWhiteSpace(comp) == 0){//printf("matched notWhiteSpace %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->wordChar == true){//printf("trying wordChar %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkWordChar(comp) == 0){//printf("matched wordChar %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }

    if(node->notWordChar == true){//printf("trying notWordChar %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkNotWordChar(comp) == 0){//printf("matched notWordChar %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }

    return validChar;
}

int checkAllButNewLine(char comp){
    char compare[] = {'\n'};
    int length = 1;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int checkEverything(char comp){
    return 0;
}

int checkDecimalDigit(char comp){
    char compare[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int length = 10;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return 0;
        }
    }

    return -1;
}

int checkNotDecimalDigit(char comp){
    char compare[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int length = 10;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int checkWhiteSpace(char comp){
    char compare[] = {' ', '\t'};
    int length = 2;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return 0;
        }
    }

    return -1;
}

int checkNotWhiteSpace(char comp){
    char compare[] = {' ', '\t'};
    int length = 2;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int checkWordChar(char comp){
    char compare[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'};
    int length = 63;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return 0;
        }
    }

    return -1;
}

int checkNotWordChar(char comp){
    char compare[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'};
    int length = 63;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}


void NDR_InitRegex(NDR_Regex* cRegex){
    cRegex->beginString = false;
    cRegex->endString = false;
    cRegex->isEmpty = false;
    cRegex->start = malloc(sizeof(NDR_RegexNode));
    NDR_InitRegexNode(cRegex->start);
    cRegex->start->start = true;
}

void NDR_InitRegexNode(NDR_RegexNode* node){
    node->start = false;
    node->end = false;
    node->wordStart = false;
    node->wordEnd = false;
    node->optionalPath = false;
    node->repeatPath = false;
    node->orPath = false;
    node->minMatches = 1;
    node->maxMatches = 1;

    node->wordReference = NULL;

    node->allButNewLine = false;
    node->everything = false;
    node->nothing = false;
    node->decimalDigit = false;
    node->notDecimalDigit = false;
    node->whiteSpace = false;
    node->notWhiteSpace = false;
    node->wordChar = false;
    node->notWordChar = false;

    node->numberOfChars = 0;
    node->memoryAllocated = 20;
    node->acceptChars = malloc(node->memoryAllocated);

    node->numberOfChildren = 0;
    node->memoryAllocatedChildren = 5;
    node->children = malloc(sizeof(NDR_RegexNode*) * node->memoryAllocatedChildren);
    NDR_RegexNode* newNode = malloc(sizeof(NDR_RegexNode));
    NDR_AddRNodeChild(node, newNode);
}


bool NDR_IsRNodeEmpty(NDR_RegexNode* node){
    if(node->start == false &&
    node->end == false &&
    node->wordStart == false &&
    node->wordEnd == false &&
    node->optionalPath == false &&
    node->repeatPath == false &&
    node->orPath == false &&
    node->minMatches == 1 &&
    node->maxMatches == 1 &&
    node->wordReference == NULL &&
    node->allButNewLine == false &&
    node->everything == false &&
    node->nothing == false &&
    node->decimalDigit == false &&
    node->notDecimalDigit == false &&
    node->whiteSpace == false &&
    node->notWhiteSpace == false &&
    node->wordChar == false &&
    node->notWordChar == false &&
    node->numberOfChars == 0 &&
    node->numberOfChildren == 1){
        return true;
    }
    return false;
}

bool NDR_IsRNodeSetForComparison(NDR_RegexNode* node){
    if(node->allButNewLine == true ||
    node->everything == true ||
    node->nothing == true ||
    node->decimalDigit == true ||
    node->notDecimalDigit == true ||
    node->whiteSpace == true ||
    node->notWhiteSpace == true ||
    node->wordChar == true ||
    node->notWordChar == true ||
    node->numberOfChars > 0){
        return true;
    }
    return false;
}


void NDR_AddRNodeChar(NDR_RegexNode* node, char character){
    if(node->numberOfChars > node->memoryAllocated - 5){
        node->memoryAllocated = node->memoryAllocated * 2;
        node->acceptChars = realloc(node->acceptChars, node->memoryAllocated);
    }
    node->acceptChars[node->numberOfChars] = character;
    node->numberOfChars++;
}

void NDR_AddRNodeChild(NDR_RegexNode* node, NDR_RegexNode* child){
    if(node->numberOfChildren > node->memoryAllocatedChildren - 2){
        node->memoryAllocatedChildren = node->memoryAllocatedChildren * 2;
        node->children = realloc(node->children, sizeof(NDR_RegexNode*) * node->memoryAllocatedChildren);
    }
    node->children[node->numberOfChildren] = child;
    node->numberOfChildren++;
}

bool NDR_RNodeDuplicate(NDR_RegexNode* address, NDR_RegexNode** nodes, size_t index){
    for(size_t i = 0; i < index; i++){
        if(address == nodes[i]){
            return true;
        }
    }
    return false;
}

// Utility function to initialize the stack
void NDR_InitRNodeStack(NDR_RNodeStack* ndrstack){
    ndrstack->memoryAllocated = 50;
    ndrstack->nodes = malloc(ndrstack->memoryAllocated * sizeof(NDR_RegexNode*));
    ndrstack->numNodes = 0;
}

// Utility function to return the size of the stack
size_t NDR_RNodeStackSize(NDR_RNodeStack* ndrstack){
    return ndrstack->numNodes;
}

// Utility function to check if the stack is empty or not
bool NDR_RNodeStackIsEmpty(NDR_RNodeStack* ndrstack) {
    return ndrstack->numNodes <= 0;
}

// Utility function to add an element `x` to the stack
void NDR_RNodeStackPush(NDR_RNodeStack* ndrstack, NDR_RegexNode* node){
    if(ndrstack->numNodes > ndrstack->memoryAllocated - 5){
        ndrstack->memoryAllocated = ndrstack->memoryAllocated * 2;
        ndrstack->nodes = realloc(ndrstack->nodes, sizeof(NDR_RegexNode*) * ndrstack->memoryAllocated);
    }
    ndrstack->nodes[ndrstack->numNodes++] = node;
}

NDR_RegexNode* NDR_RNodeStackPeek(NDR_RNodeStack* ndrstack){
    if(NDR_RNodeStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->nodes[ndrstack->numNodes - 1];
}


NDR_RegexNode* NDR_RNodeStackPop(NDR_RNodeStack* ndrstack){
    if (NDR_RNodeStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->nodes[--ndrstack->numNodes];
}

void NDR_RNodeStackSet(NDR_RNodeStack* ndrstack, NDR_RegexNode* node){
    ndrstack->nodes[ndrstack->numNodes - 1] = node;
}




void NDR_DestroyRegexNode(NDR_RegexNode* node){
    free(node->acceptChars);
    //for(int x = 0; x < node->numberOfChildren; x++){
    //    free(node->children[x]);
    //}
    free(node->children);
}

// Free the memory associated with items within the regex struct
void NDR_DestroyRegex(NDR_Regex* graph){
    free(graph->start);
}

void NDR_DestroyRegexStack(NDR_RNodeStack* stack){
    free(stack->nodes);
}

void NDR_DestroyRegexTracker(NDR_RegexTracker* tracker){
    free(tracker->reference);
}

void NDR_DestroyRegexTrackerStack(NDR_TrackerStack* stack){
    free(stack->references);
}

/*void NDR_FreeRNodeStack(NDR_RNodeStack* ndrstack){
    for(size_t x = 0; x < ndrstack->numNodes; x++){
        NDR_DestroyRegexNode(ndrstack->nodes[x]);
        free(ndrstack->nodes[x]);
    }
    free(ndrstack->nodes);
}

void NDR_DestroyRegexGraph(NDR_RegexNode* head){

    if(head != NULL){
        size_t duplicatePostDepthTrackerCount = 0;

        NDR_RNodeStack* depthTracker = malloc(sizeof(NDR_RNodeStack));
        NDR_InitRNodeStack(depthTracker);
        NDR_RegexNode** duplicateTracker = malloc(sizeof(NDR_RegexNode*) * 1000);
        NDR_RegexNode* follow = head;

        NDR_RNodeStackPush(depthTracker, follow);

        size_t count;
        while(!NDR_RNodeStackIsEmpty(depthTracker)){
            count = 0;
            while(count < follow->numberOfChars){
                if(!NDR_RNodeDuplicate(follow->child, duplicateTracker, duplicatePostDepthTrackerCount)){
                    follow = follow->child;
                    NDR_RNodeStackPush(depthTracker, follow);
                    count = 0;
                    continue;
                }
                count++;
            }

            if(!NDR_RNodeDuplicate(follow, duplicateTracker, duplicatePostDepthTrackerCount)){
                duplicateTracker[duplicatePostDepthTrackerCount++] = (follow);
            }

            NDR_RNodeStackPop(depthTracker);

            if(!NDR_RNodeStackIsEmpty(depthTracker))
                follow = NDR_RNodeStackPeek(depthTracker);
        }

        NDR_FreeRNodeStack(depthTracker);
        free(depthTracker);
        free(duplicateTracker);
    }

}
*/




void NDR_InitRTracker(NDR_RegexTracker* ref, NDR_RegexNode* node){
    ref->reference = node;
    ref->numberOfRepeats = 0;
    ref->currentChild = 0;
    ref->stringPosition = 0;
}


// Utility function to initialize the stack
void NDR_InitTrackerStack(NDR_TrackerStack* ndrstack){
    ndrstack->memoryAllocated = 50;
    ndrstack->references = malloc(ndrstack->memoryAllocated * sizeof(NDR_RegexTracker*));
    ndrstack->numNodes = 0;
}

// Utility function to return the size of the stack
size_t NDR_TrackerStackSize(NDR_TrackerStack* ndrstack){
    return ndrstack->numNodes;
}

// Utility function to check if the stack is empty or not
bool NDR_TrackerStackIsEmpty(NDR_TrackerStack* ndrstack) {
    return ndrstack->numNodes <= 0;
}

// Utility function to add an element `x` to the stack
void NDR_TrackerStackPush(NDR_TrackerStack* ndrstack, NDR_RegexTracker* node){
    if(ndrstack->numNodes > ndrstack->memoryAllocated - 5){
        ndrstack->memoryAllocated = ndrstack->memoryAllocated * 2;
        ndrstack->references = realloc(ndrstack->references, sizeof(NDR_RegexTracker*) * ndrstack->memoryAllocated);
    }
    ndrstack->references[ndrstack->numNodes++] = node;
}

NDR_RegexTracker* NDR_TrackerStackPeek(NDR_TrackerStack* ndrstack){
    if(NDR_TrackerStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->references[ndrstack->numNodes - 1];
}


NDR_RegexTracker* NDR_TrackerStackPop(NDR_TrackerStack* ndrstack){
    if (NDR_TrackerStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->references[--ndrstack->numNodes];
}
