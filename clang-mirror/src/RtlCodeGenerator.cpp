

#include "ReflectionMeta.h"
#include "RTLCodeGenerator.h"

namespace clmirror 
{
    RtlCodeGenerator::RtlCodeGenerator(const std::string& pSrcFile)
        :m_srcFile(pSrcFile)
    { }

    void RtlCodeGenerator::addRtlRecord(const RtlFunction & pFnMeta) const
    {
        auto& userType = [&]()-> RtlRecord&
        {
            const auto& itr = m_recordsMap.find(pFnMeta.m_record);
            if (itr == m_recordsMap.end())
            {
                auto& userType = m_recordsMap.emplace(pFnMeta.m_record,
                    RtlRecord{
                        .typeStr = pFnMeta.m_record,
                        .methods = RtlRecord::MemberFnsMap()
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


    void RtlCodeGenerator::addFunction(MetaKind pMetaKind, const std::string& pHeaderFile, const std::string& pRecord,
                                       const std::string& pFnName, const std::vector<std::string>& pParamTypes) const
    {
        if (pMetaKind == MetaKind::NonMemberFn)
        {
            m_freeFnsMap.emplace(pFnName, (RtlFunction{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_record = pRecord,
                    .m_function = pFnName,
                    .m_argTypes = pParamTypes
            }));
        }
        else if (pMetaKind != MetaKind::None)
        {
            addRtlRecord( RtlFunction{
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