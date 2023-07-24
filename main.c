#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "ndr_regex.c"


int main()
{
    char testRegex1[] = "[ta][ec][se][ts]";
    char testRegex2[] = "$[ta][ec][se][ts]%";
    char testRegex3[] = "$[ta][ec][se]?[ts]%";
    char testRegex4[] = "[ta][ec]+[se]*[ts]";
    char testRegex5[] = "$[ta][ec]{2}[se]{2,3}[ts]%";
    char testRegex6[] = "$\\d[\\e][se]*[tu]";
    char testRegex7[] = "$[a-z][Z-A][2-8][ts]";
    char testRegex8[] = "$[te](asd)+t%";
    char testRegex9[] = "$[te](asd)*t%";
    char testRegex10[] = "$[te](asd)?t%";
    char testRegex11[] = "$[te](asd)|(ghj)t%";
    char testRegex12[] = "$[te]((asd){2,4})|((ghj)*)|(qwe)t%";
    char testRegex13[] = "[ta][ec]{2}[se]{2,3}[ts]%";
    char testRegex14[] = "\\d[\\e][se]*[tu]";
    char testRegex15[] = "[te](asd)?t";
    char testRegex16[] = "[te]((asd){2,4})|((ghj)+)|(qwe)t";
    char testRegex17[] = "\\d[\\e][\n]*[tu]";
    char testRegex18[] = "(tqew[rt])?[\\e][tu](tqew[rt])";
    char testRegex19[] = "$(tqew[rt])[\\e][tu](tqew[rt])%";
    char testRegex20[] = "";
    char testToken1[] = "test";
    char testToken2[] = "aces";
    char testToken3[] = "ace";
    char testToken4[] = "testing";
    char testToken5[] = "aegriuh awirouh aroi";
    char testToken6[] = "aegriuh awiacesh aroi";
    char testToken7[] = "aegriuh test awiacesh aroi";
    char testToken8[] = "";
    char testToken9[] = "ytest";
    char testToken10[] = "ytgest";
    char testToken11[] = "tet";
    char testToken12[] = "tt";
    char testToken13[] = "teessst";
    char testToken14[] = "teesst";
    char testToken15[] = "5:esst";
    char testToken16[] = "dG6t";
    char testToken17[] = "dG1t";
    char testToken18[] = "easdasdt";
    char testToken19[] = "et";
    char testToken20[] = "easdt";
    char testToken21[] = "eghjt";
    char testToken22[] = "eghjghjghjt";
    char testToken23[] = "eqweqwet";
    char testToken24[] = "eqwet";
    char testToken25[] = "wtavre aeargaccsess";
    char testToken26[] = "wtavre aeargaccsessretht";
    char testToken27[] = "UOG7ue5tsukf";
    char testToken28[] = "UOG7us";
    char testToken29[] = "ctyggttvytf";
    char testToken30[] = "8654egkjghjghjteghjghjghjtiuho";
    char testToken31[] = "c8*\n\n\nu";
    char testToken32[] = "rniogp tqewt(ttqewtxhm";
    char testToken33[] = "tqewt(ttqewt";
    char testToken34[] = "";


    NDR_Regex* test1 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test2 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test3 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test4 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test5 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test6 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test7 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test8 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test9 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test10 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test11 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test12 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test13 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test14 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test15 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test16 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test17 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test18 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test19 = malloc(sizeof(NDR_Regex));
    NDR_Regex* test20 = malloc(sizeof(NDR_Regex));
    NDR_InitRegex(test1);
    NDR_InitRegex(test2);
    NDR_InitRegex(test3);
    NDR_InitRegex(test4);
    NDR_InitRegex(test5);
    NDR_InitRegex(test6);
    NDR_InitRegex(test7);
    NDR_InitRegex(test8);
    NDR_InitRegex(test9);
    NDR_InitRegex(test10);
    NDR_InitRegex(test11);
    NDR_InitRegex(test12);
    NDR_InitRegex(test13);
    NDR_InitRegex(test14);
    NDR_InitRegex(test15);
    NDR_InitRegex(test16);
    NDR_InitRegex(test17);
    NDR_InitRegex(test18);
    NDR_InitRegex(test19);
    NDR_InitRegex(test20);

    if(NDR_CompileRegex(test1, testRegex1) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test1, testRegex1) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test2, testRegex2) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test3, testRegex3) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test4, testRegex4) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test5, testRegex5) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test6, testRegex6) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test7, testRegex7) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test8, testRegex8) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test9, testRegex9) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test10, testRegex10) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test11, testRegex11) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test12, testRegex12) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test13, testRegex13) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test14, testRegex14) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test15, testRegex15) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test16, testRegex16) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test17, testRegex17) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test18, testRegex18) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test19, testRegex19) != 0){
        return 1;
    }
    if(NDR_CompileRegex(test20, testRegex20) != 0){
        return 1;
    }


    if(NDR_MatchRegex(test1, testToken1) == COMPLETEMATCH)
        printf("1. Success\n");
    else
        printf("1. Fail\n");

    if(NDR_MatchRegex(test2, testToken1) == COMPLETEMATCH)
        printf("2. Success\n");
    else
        printf("2. Fail\n");

    if(NDR_MatchRegex(test1, testToken2) == COMPLETEMATCH)
        printf("3. Success\n");
    else
        printf("3. Fail\n");

    if(NDR_MatchRegex(test1, testToken3) == PARTIALMATCH)
        printf("4. Success\n");
    else
        printf("4. Fail\n");

    if(NDR_MatchRegex(test1, testToken4) == COMPLETEMATCH)
        printf("5. Success\n");
    else
        printf("5. Fail\n");

    if(NDR_MatchRegex(test2, testToken4) == NOMATCH)
        printf("6. Success\n");
    else
        printf("6. Fail\n");

    if(NDR_MatchRegex(test1, testToken5) == NOMATCH)
        printf("7. Success\n");
    else
        printf("7. Fail\n");

    if(NDR_MatchRegex(test1, testToken6) == COMPLETEMATCH)
        printf("8. Success\n");

    else
        printf("8. Fail\n");

    if(NDR_MatchRegex(test2, testToken6) == NOMATCH)
        printf("9. Success\n");
    else
        printf("9. Fail\n");

    if(NDR_MatchRegex(test2, testToken7) == NOMATCH)
        printf("10. Success\n");
    else
        printf("10. Fail\n");

    if(NDR_MatchRegex(test1, testToken9) == COMPLETEMATCH)
        printf("11. Success\n");
    else
        printf("11. Fail\n");

    if(NDR_MatchRegex(test2, testToken9) == NOMATCH)
        printf("12. Success\n");
    else
        printf("12. Fail\n");

    if(NDR_MatchRegex(test3, testToken1) == COMPLETEMATCH)
        printf("13. Success\n");
    else
        printf("13. Fail\n");

    if(NDR_MatchRegex(test3, testToken11) == COMPLETEMATCH)
        printf("14. Success\n");
    else
        printf("14. Fail\n");

    if(NDR_MatchRegex(test4, testToken12) == PARTIALMATCH)
        printf("15. Success\n");
    else
        printf("15. Fail\n");

    if(NDR_MatchRegex(test4, testToken13) == COMPLETEMATCH)
        printf("16. Success\n");
    else
        printf("16. Fail\n");

    if(NDR_MatchRegex(test4, testToken14) == COMPLETEMATCH)
        printf("17. Success\n");
    else
        printf("17. Fail\n");

    if(NDR_MatchRegex(test5, testToken12) == NOMATCH)
        printf("18. Success\n");
    else
        printf("18. Fail\n");

    if(NDR_MatchRegex(test5, testToken13) == COMPLETEMATCH)
        printf("19. Success\n");
    else
        printf("19. Fail\n");

    if(NDR_MatchRegex(test5, testToken14) == COMPLETEMATCH)
        printf("20. Success\n");
    else
        printf("20 . Fail\n");

    if(NDR_MatchRegex(test6, testToken15) == COMPLETEMATCH)
        printf("20. Success\n");
    else
        printf("20. Fail\n");

    if(NDR_MatchRegex(test7, testToken16) == COMPLETEMATCH)
        printf("21. Success\n");
    else
        printf("21. Fail\n");

    if(NDR_MatchRegex(test7, testToken17) == NOMATCH)
        printf("22. Success\n");
    else
        printf("22. Fail\n");

    if(NDR_MatchRegex(test8, testToken18) == COMPLETEMATCH)
        printf("23. Success\n");
    else
        printf("23. Fail\n");

    if(NDR_MatchRegex(test8, testToken19) == NOMATCH)
        printf("24. Success\n");
    else
        printf("24. Fail\n");

    if(NDR_MatchRegex(test9, testToken18) == COMPLETEMATCH)
        printf("25. Success\n");
    else
        printf("25. Fail\n");

    if(NDR_MatchRegex(test9, testToken19) == COMPLETEMATCH)
        printf("26. Success\n");
    else
        printf("26. Fail\n");

    if(NDR_MatchRegex(test10, testToken18) == NOMATCH)
        printf("27. Success\n");
    else
        printf("27. Fail\n");

    if(NDR_MatchRegex(test10, testToken19) == COMPLETEMATCH)
        printf("28. Success\n");
    else
        printf("28. Fail\n");

    if(NDR_MatchRegex(test11, testToken20) == COMPLETEMATCH)
        printf("29. Success\n");
    else
        printf("29. Fail\n");

    if(NDR_MatchRegex(test11, testToken21) == COMPLETEMATCH)
        printf("30. Success\n");
    else
        printf("30. Fail\n");

    if(NDR_MatchRegex(test12, testToken18) == COMPLETEMATCH)
        printf("31. Success\n");
    else
        printf("31. Fail\n");

    if(NDR_MatchRegex(test12, testToken19) == COMPLETEMATCH)
        printf("32. Success\n");
    else
        printf("32. Fail\n");

    if(NDR_MatchRegex(test12, testToken20) == NOMATCH)
        printf("33. Success\n");
    else
        printf("33. Fail\n");

    if(NDR_MatchRegex(test12, testToken21) == COMPLETEMATCH)
        printf("34. Success\n");
    else
        printf("34. Fail\n");

    if(NDR_MatchRegex(test12, testToken22) == COMPLETEMATCH)
        printf("35. Success\n");
    else
        printf("35. Fail\n");

    if(NDR_MatchRegex(test12, testToken23) == NOMATCH)
        printf("36. Success\n");
    else
        printf("36. Fail\n");

    /*if(NDR_MatchRegex(test12, testToken24) == COMPLETEMATCH)
        printf("37. Success\n");
    else
        printf("37. Fail\n");*/

    if(NDR_MatchRegex(test13, testToken25) == COMPLETEMATCH)
        printf("38. Success\n");
    else
        printf("38. Fail\n");

    if(NDR_MatchRegex(test13, testToken26) == NOMATCH)
        printf("39. Success\n");
    else
        printf("39. Fail\n");

    if(NDR_MatchRegex(test14, testToken27) == COMPLETEMATCH)
        printf("40. Success\n");
    else
        printf("40. Fail\n");

    if(NDR_MatchRegex(test14, testToken28) == PARTIALMATCH)
        printf("41. Success\n");
    else
        printf("41. Fail\n");

    if(NDR_MatchRegex(test15, testToken29) == COMPLETEMATCH)
        printf("42. Success\n");
    else
        printf("42. Fail\n");

    if(NDR_MatchRegex(test16, testToken30) == COMPLETEMATCH)
        printf("43. Success\n");
    else
        printf("43. Fail\n");

    if(NDR_MatchRegex(test17, testToken31) == COMPLETEMATCH)
        printf("44. Success\n");
    else
        printf("44. Fail\n");

    if(NDR_MatchRegex(test18, testToken32) == COMPLETEMATCH)
        printf("45. Success\n");
    else
        printf("45. Fail\n");

    if(NDR_MatchRegex(test18, testToken33) == COMPLETEMATCH)
        printf("46. Success\n");
    else
        printf("46. Fail\n");

    if(NDR_MatchRegex(test19, testToken32) == NOMATCH)
        printf("47. Success\n");
    else
        printf("47. Fail\n");

    if(NDR_MatchRegex(test19, testToken33) == COMPLETEMATCH)
        printf("48. Success\n");
    else
        printf("48. Fail\n");

    if(NDR_MatchRegex(test19, testToken34) == NOMATCH)
        printf("49. Success\n");
    else
        printf("49. Fail\n");

    if(NDR_MatchRegex(test20, testToken1) == NOMATCH)
        printf("50. Success\n");
    else
        printf("50. Fail\n");

    if(NDR_MatchRegex(test20, testToken34) == COMPLETEMATCH)
        printf("51. Success\n");
    else
        printf("51. Fail\n");

    NDR_DestroyRegex(test1);
    NDR_DestroyRegex(test2);
    NDR_DestroyRegex(test3);
    NDR_DestroyRegex(test4);
    NDR_DestroyRegex(test5);
    NDR_DestroyRegex(test6);
    NDR_DestroyRegex(test7);
    NDR_DestroyRegex(test8);
    NDR_DestroyRegex(test9);
    NDR_DestroyRegex(test10);
    NDR_DestroyRegex(test11);
    NDR_DestroyRegex(test12);
    NDR_DestroyRegex(test13);
    NDR_DestroyRegex(test14);
    NDR_DestroyRegex(test15);
    NDR_DestroyRegex(test16);
    NDR_DestroyRegex(test17);
    NDR_DestroyRegex(test18);
    NDR_DestroyRegex(test19);
    NDR_DestroyRegex(test20);
    free(test1);
    free(test2);
    free(test3);
    free(test4);
    free(test5);
    free(test6);
    free(test7);
    free(test8);
    free(test9);
    free(test10);
    free(test11);
    free(test12);
    free(test13);
    free(test14);
    free(test15);
    free(test16);
    free(test17);
    free(test18);
    free(test19);
    free(test20);


    return 0;
}


