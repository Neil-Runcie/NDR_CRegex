# NDR_CRegex

## Purpose
NDR_CRegex is a library for straightforward regular expression matching in C.

The inspiration for the NDR_CRegex library was the need for an easy to use tool for text matching within a lexical analysis tool without getting distracted by all of the options that more sophisticated regular expression engines proved.

With that being said, this library is to be used when the use case for regular expressions is not too complex and therefore a simpler and easier to pick up tool is worth the tradeoff.

Lastly, this regular expression engine does not currently support back tracking which can be important within certain applications but it will in the future.

## How To Use
The project is set up to be built using cmake.
After running cmake and make commands to build the project, the necessary libraries will be present within the "lib" folder and the header files will be present within the "include" folder.
These can be used within any c project created to access all of the features of NDR_CRegex

## Documentation
For high level documentation for the NDR_CRegex library, open the instruction.html page in your browser

For the exposed functionality of the NDR_CRegex library, open the doxygen generated documentation by going to documentation/html/index.html and opening the file in your browser
