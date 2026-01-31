

#include "ASTMeta.h"
#include "ASTCodeGenerator.h"

namespace clmirror
{
    ASTCodeGenerator::ASTCodeGenerator(const std::string& pSrcFile)
        :m_srcFile(pSrcFile)
    { }

    void ASTCodeGenerator::addRtlRecord(const ASTMetaFn & pFnMeta)
    {
        auto& userType = [&]()-> ASTMetaType&
        {
            const auto& itr = m_recordsMap.find(pFnMeta.m_record);
            if (itr == m_recordsMap.end())
            {
                auto& userType = m_recordsMap.emplace(pFnMeta.m_record,
                    ASTMetaType{
                        .typeStr = pFnMeta.m_record,
                        .methods = ASTMetaType::MemberFnsMap()
                    }).first->second;
                return userType;
            }
            else {
                auto& userType = itr->second;
                return userType;
            }
        }();
        userType.methods.emplace(pFnMeta.m_function, pFnMeta);
    }


    void ASTCodeGenerator::addFunction(MetaKind pMetaKind, const std::string& pHeaderFile, const std::string& pRecord,
                                       const std::string& pFnName, const std::vector<std::string>& pParamTypes)
    {
        if (pMetaKind == MetaKind::NonMemberFn)
        {
            m_freeFnsMap.emplace(pFnName, (ASTMetaFn{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_record = pRecord,
                    .m_function = pFnName,
                    .m_argTypes = pParamTypes
            }));
        }
        else if (pMetaKind != MetaKind::None)
        {
            addRtlRecord( ASTMetaFn{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_record = pRecord,
                    .m_function = pFnName,
                    .m_argTypes = pParamTypes
            });
        }
        m_incFiles.insert(pHeaderFile);
    }
}