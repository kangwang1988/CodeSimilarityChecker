# CodeSimilarityChecker
Code similarity checker using clang.(AST approach)
      CodeSimilarityChecker is a source-code level tool for analyzing your C/C++/Objective-C project.With the help of clang,you may dump the asttree,analyze it,optimize it,dump it.Besides, you may check if two methods/functions are the same or their similarities even when the variables vary or they would be the same after optimizing.

# Usage:
      ClangSimilarityChecker yourpathto-workspace/project -extra-arg="-isysroot/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator10.2.sdk" -extra-arg="-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1" -extra-arg="-mios-version-min=8.0"

# Notice
      Build your llvm/clang with RTTI support.

# Problems
       Passed on demo project. If you experience any problems in your project, comment this project or mailto me. I would check and solve it ASAP.
