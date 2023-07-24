

typedef enum NDR_MatchResult {
    NOMATCH, PARTIALMATCH, COMPLETEMATCH
} NDR_MatchResult;

typedef struct NDR_RegexNode NDR_RegexNode;

typedef struct NDR_Regex {
    bool initialized;
    bool beginString;
    bool endString;
    bool isEmpty;
    NDR_RegexNode* start;
} NDR_Regex;


typedef struct NDR_RNodeStack{
    NDR_RegexNode** nodes;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_RNodeStack;

/** @brief Based on a provided regex string, create a reference graph for later matching the regex to other strings
*
* @param cRegex is an NDR_Regex pointer with sufficient memory already allocated
* @param regexString is the regex pattern that will be used to create the regex graph
* @return The success status of the function. 0 for success and non-zero for error
*/
int NDR_CompileRegex(NDR_Regex* cRegex, char* regexString);
/** @brief Compare a string to a pre-compiled regex graph
*
* @param cRegex is an NDR_Regex pointer with sufficient memory already allocated that has been used previously in the NDR_CompileRegex function
* @param token is the string that will be compared to the compiled regex graph
* @return The result of the match
*/
NDR_MatchResult NDR_MatchRegex(NDR_Regex* cRegex, char* token);
/** @brief Free the memory associated with items within the regex struct
*
* @param graph is a NDR_Regex pointer that has had memory assigned to it for compilation
*/
void NDR_DestroyRegex(NDR_Regex* graph);
