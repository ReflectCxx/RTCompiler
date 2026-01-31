# Source files
set(CLREFLECT_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/ASTParser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ClangDriver.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ClangReflectActionFactory.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ClangReflectDiagnosticConsumer.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/main.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTDeclsUtils.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTVisitor.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/Logger.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTMeta.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeManager.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeGenerator.cpp"
    # Add more source files as needed
)

target_sources(${CXX_EXE_NAME} PRIVATE ${CLREFLECT_SRCS})