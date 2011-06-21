#ifndef __Facade_FieldDescription_h 
#define __Facade_FieldDescription_h

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

namespace UAKGQuery2 {

struct FieldDescription {
       CORBA::String_var     name;
       UAKGQuery2::FieldType type;
	CORBA::ULong         size;      // have sence only for types 
					// of variable length.
       CORBA::UShort         precision; // have sense only for numeric types
       CORBA::UShort         scale;     // have sense only for numeric types
public:
       FieldDescription() : type(UAKGQuery2::TypeNull),
			    size(0), precision(0), scale(0) 
      { name = CORBA::string_dup(""); }

      FieldDescription( const char* name_, const  UAKGQuery2::FieldType type_, 
	        	const CORBA::ULong size_ = 0, 
                        const CORBA::UShort  precision_ = 0,
			const CORBA::UShort scale_ = 0
                       ) : type(type_), size(size_), precision(precision_), 
                           scale(scale_)
       { name = CORBA::string_dup( name_); }

	FieldDescription(const FieldDescription& fd)
        {
	   name = CORBA::string_dup( fd.name );
	   type = fd.type;
	   size = fd.size;
	   precision = fd.precision;
	   scale = fd.scale;
       }

       void operator=( const FieldDescription& fd)
       {
	   if ( this != &fd ) {
	       name = CORBA::string_dup( fd.name );
	       type = fd.type;
	       size = fd.size;
	       precision = fd.precision;
	       scale = fd.scale;
	   }
       }

       CORBA::Boolean haveFieldSize() const
       {
         switch (type) {
            case TypeString:
            case TypeWString:
            case TypeNumeric:
            case TypeDecimal:
            case TypeRaw:
              return true;
         }
         return false;
       }


};

}; // namespace UAKGQuery2

#endif
