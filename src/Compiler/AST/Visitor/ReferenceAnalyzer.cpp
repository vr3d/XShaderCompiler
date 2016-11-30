/*
 * ReferenceAnalyzer.cpp
 * 
 * This file is part of the XShaderCompiler project (Copyright (c) 2014-2016 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "ReferenceAnalyzer.h"
#include "AST.h"


namespace Xsc
{


void ReferenceAnalyzer::MarkReferencesFromEntryPoint(Program& program)
{
    program_ = &program;
    Visit(program.entryPointRef);
}


/*
 * ======= Private: =======
 */

bool ReferenceAnalyzer::Reachable(AST* ast)
{
    if (!ast->flags(AST::isReachable))
    {
        ast->flags << AST::isReachable;
        return true;
    }
    return false;
}

/* ------- Visit functions ------- */

#define IMPLEMENT_VISIT_PROC(AST_NAME) \
    void ReferenceAnalyzer::Visit##AST_NAME(AST_NAME* ast, void* args)

IMPLEMENT_VISIT_PROC(FunctionCall)
{
    /* Mark function declaration as referenced */
    Visit(ast->funcDeclRef);

    /* Collect all used intrinsics */
    if (ast->intrinsic != Intrinsic::Undefined)
        program_->usedIntrinsics.insert(ast->intrinsic);

    VISIT_DEFAULT(FunctionCall);
}

IMPLEMENT_VISIT_PROC(VarType)
{
    if (Reachable(ast))
    {
        Visit(ast->symbolRef);
        VISIT_DEFAULT(VarType);
    }
}

IMPLEMENT_VISIT_PROC(VarIdent)
{
    if (Reachable(ast))
    {
        Visit(ast->symbolRef);
        VISIT_DEFAULT(VarIdent);
    }
}

/* --- Declarations --- */

IMPLEMENT_VISIT_PROC(VarDecl)
{
    if (Reachable(ast))
    {
        Visit(ast->declStmntRef);
        Visit(ast->bufferDeclRef);
        VISIT_DEFAULT(VarDecl);
    }
}

IMPLEMENT_VISIT_PROC(StructDecl)
{
    if (Reachable(ast))
        VISIT_DEFAULT(StructDecl);
}

IMPLEMENT_VISIT_PROC(TextureDecl)
{
    if (Reachable(ast))
        Visit(ast->declStmntRef);
}

/* --- Declaration statements --- */

IMPLEMENT_VISIT_PROC(FunctionDecl)
{
    if (Reachable(ast))
        VISIT_DEFAULT(FunctionDecl);
}

IMPLEMENT_VISIT_PROC(BufferDeclStmnt)
{
    if (Reachable(ast))
        VISIT_DEFAULT(BufferDeclStmnt);
}

IMPLEMENT_VISIT_PROC(TextureDeclStmnt)
{
    if (Reachable(ast))
        VISIT_DEFAULT(TextureDeclStmnt);
}

#undef IMPLEMENT_VISIT_PROC


} // /namespace Xsc



// ================================================================================