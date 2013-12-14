#include <llvm/Support/TargetRegistry.h>

#include <translator.hpp>
#include <debug.hpp>

namespace pa
{

    using namespace llvm;

    OwningPtr<translator> create_translator(Triple const & triple)
    {
        std::string err;
        Target const *target = TargetRegistry::lookupTarget(triple.str(), err);
        if (!target)
        {
            warn(err);
            return OwningPtr<translator>(nullptr);
        }

        switch (triple.getArch())
        {
        case Triple::x86_64:
            return OwningPtr<translator>(translator_factory<Triple::x86_64>(triple, target));
        default:
            warn("No translator for ", triple.str());
            return OwningPtr<translator>(nullptr);
        }

        return OwningPtr<translator>(nullptr);
    }

    OwningPtr<translator> create_translator(Triple::ArchType type)
    {
        return create_translator(Triple(Triple::getArchTypeName(type),
                                        Triple::getVendorTypeName(Triple::UnknownVendor),
                                        Triple::getOSTypeName(Triple::UnknownOS)));
    }

} /* namespace pa */
