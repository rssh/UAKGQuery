#include <InterBase/InterBaseQuery.h>
#include <InterBase/InterBaseQueryManager.h>

#include <strstream>

#include <Common/UAKGQueryUtils.h>

#include <InterBase/IBTransSet.h>
#include <InterBase/InterBaseLobs.h>
#include <memory>
#include <climits>
using namespace std;
using namespace UAKGQuery2;

static int counter = 0;

//short flag =0;

const bool operator < (const ISC_QUAD& l, const ISC_QUAD& r)
{
    return ((l.isc_quad_high < r.isc_quad_high)
            || ((l.isc_quad_high == r.isc_quad_high)
                && (l.isc_quad_low < r.isc_quad_low)));
};

InterBaseQuery::InterBaseQuery(const char* query_text,
               InterBaseQueryManager* interBaseQueryManager_p,
               const char* szflags
              ):UAKGQueryBase(query_text,interBaseQueryManager_p,szflags)
{
   transaction_started_ = false;
   interBaseQueryManager_p_=interBaseQueryManager_p;
   trans_handle_ = 0L;
   stmt_handle_ = 0L;
   osqlda_ = NULL;
   isqlda_ = NULL;
//   cursor_name_ = CORBA::string_dup("");
//   cursor_name_ += counter++;
   try {
     CORBA::ULong allocMode=0;
     handles_p_=interBaseQueryManager_p->getSLHndlsPool_p()->alloc(allocMode);
   }catch(const IBSessionBeginException& ex){
     std::cerr << ex.msg << std::endl;
     error_ = true;
   }
   if (error_) {
     handles_p_ = NULL;
   }
}

InterBaseQuery::~InterBaseQuery()
{
 close();
}

void InterBaseQuery::prepare_internal()
{
    int i=0;

    resultChunkSize_ = 1;

    ISC_STATUS* status = handles_p_->status();
    isc_db_handle db_handle = handles_p_->db_handle();
    int num_cols = 0;  //initial size
    short fieldLength = 0;

    /* Allocate output SQLDA of an arbitrary size. */
    osqlda_ = (XSQLDA ISC_FAR *) malloc(XSQLDA_LENGTH(num_cols));
    osqlda_->sqln = num_cols;
    osqlda_->version = 1;

    /* Allocate input SQLDA of an arbitrary size. */
    isqlda_ = (XSQLDA ISC_FAR *) malloc(XSQLDA_LENGTH(num_cols));
    isqlda_->sqln = num_cols;
    isqlda_->version = 1;

    /* Start transaction */
#ifdef CORBA_HAVE_OTS
    joinCurrentTransaction();
#endif
//    if (!transaction_started_)
    if (*current_transaction_p()==0L)
    {
      if (isc_start_transaction(status,
                                current_transaction_p(),
                                1,
                                &db_handle,
                                0,
                                NULL))
      {
          check_error(status, "isc_start_transaction");
//	  transaction_started_ = true;
      }
    }

    /* Allocate a statement. */
    *current_stmt_p() = 0L;
    if (isc_dsql_allocate_statement(status,
                                    &db_handle,
                                    current_stmt_p()))
    {
        check_error(status, "isc_dsql_allocate_statement");
    }

    /* Prepare the statement. */
    if (isc_dsql_prepare(status,
                         current_transaction_p(),
                         current_stmt_p(),
                         0,
                         queryText_,
                         SQL_DIALECT_V6,
                         osqlda_))
    {
        check_error(status, "isc_dsql_prepare");
    }

    /* Describe bind statement */
    if (isc_dsql_describe_bind(status,
                              current_stmt_p(),
                              SQL_DIALECT_V6,
                              isqlda_))
    {
        check_error(status, "isc_dsql_describe_bind");
    }


    /* realloc isqlda_ if it is neccesary */
    if (isqlda_->sqld > isqlda_->sqln)
    {
        int n = isqlda_->sqld;
        free(isqlda_);
        isqlda_ = (XSQLDA *)malloc(XSQLDA_LENGTH(n));
        isqlda_->sqln = n;
        isqlda_->version = SQLDA_VERSION1;
        isc_dsql_describe_bind(status, &stmt_handle_, SQL_DIALECT_V6, isqlda_);
    }

    /* Describe the statement. */
    if (isc_dsql_describe(status,
                          current_stmt_p(),
                          SQL_DIALECT_V6,
                          osqlda_))
    {
        check_error(status, "isc_dsql_describe");
    }

    if (osqlda_->sqln < osqlda_->sqld)
    {
        num_cols = osqlda_->sqld;
        free(osqlda_);
        osqlda_= (XSQLDA ISC_FAR *) malloc(XSQLDA_LENGTH(num_cols));
        osqlda_->sqln = num_cols;
        osqlda_->version = 1;

        /* Re-describe the statement. */
        if (isc_dsql_describe(status,
                              current_stmt_p(),
                              SQL_DIALECT_V6,
                              osqlda_))
        {
            check_error(status, "isc_dsql_describe");
        }
    }

    selectFlg_ = (osqlda_->sqld != 0);

    RecordSet_var r = buildBindDescription(*osqlda_);
    setResultBindDescription(r.in());

    r = buildBindDescription(*isqlda_);
    setParamsBindDescription(r);
     
    paramsBindSet_p_->allocate();
}

RecordSet* InterBaseQuery::buildBindDescription(const XSQLDA& sqlda)
{
    RecordSet_var resultSet = new RecordSetImpl;
    int i = 0;
    short fieldLength = 0;

    /* Fill column names, types, and lengths. */
    for (i = 0; i < sqlda.sqld; i++)
    {
        int name_length = strlen(sqlda.sqlvar[i].sqlname);
        char* name = CORBA::string_alloc(name_length+1);
        strncpy(name,(const char*)sqlda.sqlvar[i].sqlname,name_length);
        name[name_length]='\0';
        fieldLength = sqlda.sqlvar[i].sqllen;
        FieldType ftype = internalDType2valueType(sqlda.sqlvar[i].sqltype,
                                          fieldLength,
                                          sqlda.sqlvar[i].sqllen,
                                          sqlda.sqlvar[i].sqlscale);

        CORBA::ULong  col = resultSet->getNColumns();

	resultSet->addColumn(name, ftype);
	CORBA::string_free(name);

        resultSet->setFieldSizeAt(col,fieldLength);

        if ((ftype == TypeNumeric) || (ftype == TypeDecimal)) {
           resultSet->setNumericFieldPropertiesAt( col, 
						 sqlda.sqlvar[i].sqllen, 
						 sqlda.sqlvar[i].sqlscale );
        }
    }
    return resultSet._retn();
}


void InterBaseQuery::execute_internal()
{
    for (LobHandles::iterator it = lob_handles_.begin();
            it != lob_handles_.end(); ++it) {
        close_lob_handle((*it).second);
    }
    lob_handles_.clear();

    current_param_record_ = 0;

    ISC_STATUS* status = handles_p_->status();

//    UAKGQueryUtils::printRecordDescription(cerr
//            , paramsBindSet_p_->getRecordDescriptionRef());

    int i=0;
    for (i = 0; i < osqlda_->sqld; i++)
    {
       switch(osqlda_->sqlvar[i].sqltype){
           case SQL_VARYING:
	   case SQL_VARYING+1:
	       osqlda_->sqlvar[i].sqltype = SQL_TEXT+1;
       }
	osqlda_->sqlvar[i].sqldata = (char ISC_FAR*) resultBindSet_p_->getColAddr(i);
    CORBA::Short* flag = resultBindSet_p_->getNullIndicatorPtr(0,i);
    *flag = 0;
	osqlda_->sqlvar[i].sqlind = (short*)flag;
//       osqlda_->sqlvar[i].sqlind = (short*) resultBindSet_p_->getNullIndicatorPtr(0,i);
//       *resultBindSet_p_->getNullIndicatorPtr(0,i) = 0;
    }

    bind_next_param();
    if (!selectFlg_)
    {
      while(paramsBindSet_p_->getNRows() > current_param_record_)
      {
        bind_next_param();
      }
    }
    executed_ = true;
}

CORBA::Boolean InterBaseQuery::retrieve_internal()
{
    if (fetchedNRows_ < retrievedNRows_) {
       return !retrieved_;  // i. e. we must "physically" call retrieve_internal
                            // only if we fetch all before.
    }
    if (!selectFlg_) return false;
    if (!retrieved_)
    {
        ISC_STATUS* status = handles_p_->status();
        int ret_code = isc_dsql_fetch(status, current_stmt_p(), 1, osqlda_);
        if (ret_code == 100){
            if (paramsBindSet_p_->getNRows() > current_param_record_){
                bind_next_param();
		retrieved_ = false;
           }else{
                retrieved_ = true;
            }
        }else{
            if (ret_code){
                check_error(status, "isc_dsql_fetch");
                throwQueryProcessingError(3,"Can not fetch records.",false);
            }else{
                retrievedNRows_++;
            }
        }
    }
    return !retrieved_;
}

void InterBaseQuery::close()
{
 if (!closed_) {
      for (LobHandles::iterator it = lob_handles_.begin();
            it != lob_handles_.end(); ++it) {
        close_lob_handle((*it).second);
      }
      lob_handles_.clear();

    ISC_STATUS* status = handles_p_->status();

   if (stmt_handle_ != 0L)
   {
       if (isc_dsql_free_statement(status,
                                   &stmt_handle_,
                                   DSQL_drop))
       {
           check_error(status, "isc_dsql_free_statement");
       }
   }

   if (trans_handle_ != 0L)
   {
       if (isc_commit_transaction(status,
                                  &trans_handle_))
       {
	   transaction_started_ = false;
           check_error(status, "isc_commit_transaction");
       }
   }

   if (handles_p_ != NULL) {
      interBaseQueryManager_p_->getSLHndlsPool_p()->dealloc(handles_p_);
      handles_p_ = NULL;
   }
   if (osqlda_ != NULL)
     free(osqlda_);
   if (isqlda_ != NULL)
     free(isqlda_);

   UAKGQueryBase::close();
 }
 closed_ = true;
}

#ifdef  _MSC_VER
#pragma warning(disable:4715)           // disable not all path return value
// #pragma warning(default:4715)
#endif  // _MSC_VER


FieldType  InterBaseQuery::internalDType2valueType
      (short dtype,
       short& dlen,
       short precision,
       short scale)
{
    switch(dtype){
        case  SQL_ARRAY:
        case  SQL_ARRAY+1:
          {
            return TypeClob;
          }
        case  SQL_BLOB:
        case  SQL_BLOB+1:
            return TypeBlob;
        case  SQL_TEXT:
        case  SQL_TEXT+1:  
            return TypeString;
        case  SQL_DATE:
        case  SQL_DATE+1:
        case  SQL_TYPE_DATE: 
        case  SQL_TYPE_DATE+1:
        case  SQL_TYPE_TIME:
        case  SQL_TYPE_TIME+1:
            return TypeDateTime;
        case  SQL_SHORT:
        case  SQL_SHORT+1:
            dlen = sizeof(CORBA::Short);
            return TypeShort;
        case  SQL_LONG:
        case  SQL_LONG+1:
            dlen = sizeof(CORBA::Long);
            return TypeLong;
        case  SQL_DOUBLE:
        case  SQL_DOUBLE+1:
            dlen = sizeof(CORBA::Double);
            return TypeDouble;
        case  SQL_FLOAT:
        case  SQL_FLOAT+1:
        case  SQL_D_FLOAT:
        case  SQL_D_FLOAT+1:
            dlen = sizeof(CORBA::Float);
            return TypeFloat;
        case  SQL_INT64:
        case  SQL_INT64+1:
            dlen=interBaseDBFormatHandler.decimal_length();
            return TypeNumeric;
        case  SQL_VARYING:
        case  SQL_VARYING+1:
            return TypeString;
        default:
            cerr << "dtype = " << dtype << endl;
            throwQueryProcessingError(2001,"unknown internal datatype",false);
    }
}

CORBA::ULong
InterBaseQuery::getFieldSize(UAKGQuery2::RecordSet* rs, CORBA::ULong col) const
{

    if (rs->haveFieldSizeAt(col))
        return rs->getFieldSizeAt(col);

    switch ( rs->getFieldTypeAt(col) ) {
      case TypeBoolean:
            return sizeof(CORBA::Boolean);
      case TypeChar:
      case TypeCharacter:
            return sizeof(CORBA::Char);
      case TypeOctet:
            return sizeof(CORBA::Octet);
      case TypeShort:
      case TypeSmallInt:
      case TypeUShort:
            return sizeof(CORBA::Short);
      case TypeLong:
      case TypeInteger:
      case TypeULong:
            return sizeof(CORBA::Long);
      case TypeFloat:
      case TypeReal:
            return sizeof(CORBA::Float);
      case TypeDouble:
      case TypeDoublePrecision:
            return sizeof(CORBA::Double);
      case TypeDateTime:
            return sizeof(DateTime);
    }
    return 0;
}

#ifdef  _MSC_VER
#pragma warning(default:4715) //not all path return value
#endif  // _MSC_VER


void InterBaseQuery::bind_next_param()
{
    CORBA::ULong size;
    if (isqlda_->sqln != paramsBindSet_p_->getNColumns())
    {
      throwQueryProcessingError(2001,"Number of query parameters doesn't correspond to actual parameters.",false);
    }
    if (paramsBindSet_p_->getNRows()>0)
    {
        int i = 0;
        for (i = 0; i < isqlda_->sqld; i++)
        {
            check_type(isqlda_->sqlvar[i].sqltype, paramsBindSet_p_->getFieldDescription(i).type);
            isqlda_->sqlvar[i].sqldata = (char ISC_FAR*) paramsBindSet_p_->getRawMemBuffPtr(current_param_record_,i);
            RecordSet_var rs = paramsBindSet_p_->getRecordSet(); 
	    if ( rs == NULL ) continue;
	    size = getFieldSize(rs, i);
            switch(isqlda_->sqlvar[i].sqltype){
              case SQL_VARYING:
              case SQL_VARYING+1:
              case SQL_TEXT:
                  {
                     isqlda_->sqlvar[i].sqltype = SQL_TEXT+1;
		     int len = size;
		     if (len > strlen((char*) paramsBindSet_p_->getColAddr(i)))
		       len = strlen((char*) paramsBindSet_p_->getColAddr(i));
                     isqlda_->sqlvar[i].sqllen = len;
                     break;
                  }
              case SQL_LONG:
                  {
                     isqlda_->sqlvar[i].sqltype = SQL_LONG+1;
                     isqlda_->sqlvar[i].sqllen = size;
                     break;
                  }
	      case SQL_BLOB:
	      case SQL_BLOB+1:
	          {
		      isqlda_->sqlvar[i].sqltype = SQL_BLOB+1;
		      isqlda_->sqlvar[i].sqllen = sizeof(ISC_QUAD);
		
		      break;
	          }
	      default:
	          {
		     isqlda_->sqlvar[i].sqllen = size;
		  }
            }
            isqlda_->sqlvar[i].sqlind  = &flag_;
            flag_ = 0;
        }

        current_param_record_++;
    }

    ISC_STATUS* status = handles_p_->status();

    if (isc_dsql_free_statement(status,
                                current_stmt_p(),
                                DSQL_close))
    {
        if (isc_sqlcode(status) != -501)
            check_error(status, "isc_dsql_free_statement");
    }

    cursor_name_ = CORBA::string_alloc(sizeof(isc_tr_handle*)+1);
    memcpy(cursor_name_, current_transaction_p(), sizeof(isc_tr_handle*));
    cursor_name_[sizeof(isc_tr_handle*)] = '\0';
    if (isc_dsql_set_cursor_name(status,
                                 current_stmt_p(),
                                 cursor_name_,
                                 0))
    {
        check_error(status, "isc_dsql_set_cursor_name");
    }

    XSQLDA* in = isqlda_;
    if (isqlda_->sqln == 0){
       in = NULL;
    }
    if (isc_dsql_execute2(status,
                          current_transaction_p(),
                          current_stmt_p(),
                          1,
                          in,
                          NULL))
    {
        check_error(status, "isc_dsql_execute2");
    }

}

void InterBaseQuery::check_error(ISC_STATUS* status, const char* operation)
{
  cerr << "\n[ PROBLEM ON " << operation << endl;
  isc_print_status(status);
  cerr << "SQLCODE: " << isc_sqlcode(status) << " ]" << endl;
  cerr << "SQL: " << queryText_ << endl;

  throwQueryProcessingError(isc_sqlcode(status), operation, true);
}


void InterBaseQuery::fillDBError()
{
  ISC_STATUS* status = handles_p_->status();
  ostrstream ostr;
  isc_interprete(error_buffer_, &status);
  ostr << error_buffer_ << endl;
  error_buffer_[0] = '-';
  while(isc_interprete(error_buffer_ + 1,&status)) /* More messages? */
  {
      ostr << error_buffer_ << endl;
  }
  ostr << ends;

  strncpy(error_buffer_, ostr.str(), 400);
  ostr.rdbuf()->freeze(0);

  queryError_.dbCode=isc_sqlcode(status);
  queryError_.why=CORBA::string_dup(error_buffer_);
  //queryError_.sqlString=CORBA::string_dup(queryText_);
  cerr << queryError_.why << endl;
}

void InterBaseQuery::check_type(int db_type, int internal_type)
{
  switch(db_type)
  {
        case  SQL_ARRAY:
        case  SQL_ARRAY+1:
            if (internal_type != TypeClob)
              throw UAKGQuery2::InvalidFieldType();
            else
              break;
        case  SQL_BLOB:
        case  SQL_BLOB+1:
            if (internal_type != TypeBlob)
              throw UAKGQuery2::InvalidFieldType();
            else
              break;
        case  SQL_TEXT:
        case  SQL_TEXT+1:
        case  SQL_VARYING:
        case  SQL_VARYING+1:
            switch(internal_type)
            {
              case TypeString:
              case TypeBoolean:
              case TypeChar:
              case TypeOctet:
              case TypeCharacter:
                  break;
              default: throw UAKGQuery2::InvalidFieldType();
            }
            break;
        case  SQL_DATE:
        case  SQL_DATE+1:
        case  SQL_TYPE_DATE:
        case  SQL_TYPE_DATE+1:
        case  SQL_TYPE_TIME:
        case  SQL_TYPE_TIME+1:
            if (internal_type != TypeDateTime)
              throw UAKGQuery2::InvalidFieldType();
            else
              break;
        case  SQL_SHORT:
        case  SQL_SHORT+1:
            switch(internal_type)
            {
              case TypeShort:
              case TypeUShort:
              case TypeSmallInt:
                 break;
              default: throw UAKGQuery2::InvalidFieldType();
            }
            break;
        case  SQL_LONG:
        case  SQL_LONG+1:
            switch(internal_type)
            {
              case TypeLong:
              case TypeULong:
              case TypeInteger:
	      case TypeDecimal:
	      case TypeNumeric:
                break;
              default: throw UAKGQuery2::InvalidFieldType();
            }
            break;
        case  SQL_DOUBLE:
        case  SQL_DOUBLE+1:
            switch(internal_type)
	    {
	      case TypeDouble:
	      case TypeDoublePrecision:
	      case TypeNumeric:
	      case TypeDecimal:
	        break;
	      default:
                throw UAKGQuery2::InvalidFieldType();
	    }
	    break;
        case  SQL_FLOAT:
        case  SQL_FLOAT+1:
        case  SQL_D_FLOAT:
        case  SQL_D_FLOAT+1:
            switch(internal_type)
            {
              case TypeFloat:
              case TypeReal:
                break;
              default: throw UAKGQuery2::InvalidFieldType();
            }
            break;
        case  SQL_INT64:
        case  SQL_INT64+1:
            switch(internal_type)
            {
              case TypeNumeric:
              case TypeDecimal:
              case TypeDoublePrecision:
                break;
              default: throw UAKGQuery2::InvalidFieldType();
            }
  }
}

isc_tr_handle* InterBaseQuery::current_transaction_p()
{
#ifdef CORBA_HAVE_OTS
    IBTransSet::ByTransactionIterator it = IBTransSet::byTransaction_.find(const_cast<XID*>(&transactionXID_));
    IBTransSet::TransRecord* tr;
    if (it==IBTransSet::byTransaction_.end_()) {
      throw NoCurrentTransaction();
    }else{
      tr = (*it).second;
      return &tr->trans_handle;
    }
#else
    return &trans_handle_;
#endif
}

isc_stmt_handle* InterBaseQuery::current_stmt_p()
{
#ifdef CORBA_HAVE_OTS
    IBTransSet::ByTransactionIterator it = IBTransSet::byTransaction_.find(const_cast<XID*>(&transactionXID_));
    IBTransSet::TransRecord* tr;
    if (it==IBTransSet::byTransaction_.end_()) {
      throw NoCurrentTransaction();
    }else{
      tr = (*it).second;
      return &tr->stmt_handle;
    }
#else
    return &stmt_handle_;
#endif
}


//   BLOB

const void* InterBaseQuery::open_lob_for_writing(size_t& length)
{
    ISC_QUAD blob_id;
    isc_blob_handle blob_handle;
	blob_handle = NULL;
	ISC_STATUS* status = handles_p_->status();
	isc_db_handle db_handle = handles_p_->db_handle();
	if (isc_create_blob2(
		status,
		&db_handle,
		current_transaction_p(),
		&blob_handle,/* set by this function to refer to the Blob */
		&blob_id, /* Blob ID put into out_sqlda by isc_dsql_fetch() */
		0,     /* BPB length = 0; no filter will be used */
		NULL     /* NULL BPB, since no filter will be used */))
	{
		check_error(status, "isc_create_blob2");
	}
    LobHandles::iterator it = (lob_handles_.insert(
            make_pair(blob_id, blob_handle))).first;

    length = sizeof(ISC_QUAD);
    return &((*it).first);
}

void InterBaseQuery::close_lob_handle(isc_blob_handle& handle)
{
	ISC_STATUS* status = handles_p_->status();

	if (isc_close_blob(status, &handle))
	{
		check_error(status, "isc_close_blob");
	}
}

void InterBaseQuery::write_lob_chunk(ISC_QUAD& id
        , const char* data, unsigned long length)
{
    isc_blob_handle handle = lob_handles_[id];

	// Write segment to blob
	ISC_STATUS* status = handles_p_->status();
    for (unsigned long i = length/USHRT_MAX; i > 0; --i, data += USHRT_MAX) {
        if (isc_put_segment(status, &handle, USHRT_MAX
                , const_cast<char*>(data))) {
		    check_error(status, "isc_put_segment");
        }
    }
    unsigned short last_length = length%USHRT_MAX;
    if (last_length > 0) {
        if (isc_put_segment(status, &handle, last_length
                , const_cast<char*>(data))) {
		    check_error(status, "isc_put_segment");
        }
    }
}

unsigned long InterBaseQuery::get_lob_length(isc_blob_handle& handle)
{
    char blob_items[] = {isc_info_blob_total_length};
    char res_buffer[20], *p, item;
    short length;
    long total_length = 0L;
	ISC_STATUS* status_vector = handles_p_->status();
    isc_blob_info(
        status_vector,
        &handle, /* Set in isc_open_blob2() call above. */
        sizeof(blob_items),/* Length of item-list buffer. */
        blob_items, /* Item-list buffer. */
        sizeof(res_buffer),/* Length of result buffer. */
        res_buffer /* Result buffer */
    );
    if (status_vector[0] == 1 && status_vector[1]) {
        /* An error occurred. */
        isc_print_status(status_vector);
        return 0;
    };
    /* Extract the values returned in the result buffer. */
    for (p = res_buffer; *p != isc_info_end ;) {
        item = *p++;
        length = (short)isc_vax_integer(p, 2);
        p += 2;
        switch (item) {
            case isc_info_blob_total_length:
                total_length = isc_vax_integer(p, length);
                break;
            case isc_info_truncated:
                /* handle error */
                break;
            default:
                break;
        }
        p += length;
    };
    return total_length;
}

char* InterBaseQuery::read_lob_chunk(ISC_QUAD& id
        , CORBA::ULong chunkSize, unsigned long& actual_length,
        CORBA::Boolean& more,
        FieldType t)
{
    LobHandles::iterator it = lob_handles_.find(id);
    isc_blob_handle handle;
    if (it != lob_handles_.end()) {
        handle = (*it).second;
    } else {
        handle = open_lob_for_reading(id);
        lob_handles_[id] = handle;
    }
	// Read segment from blob
	ISC_STATUS* status = handles_p_->status();

    bool isEntireRead = false;
    if (chunkSize == 0) {
        chunkSize = get_lob_length(handle);
        isEntireRead = true;
    }

    char* blob_segment;
    switch (t) {
        case TypeBlob:
            blob_segment = new char[chunkSize];
            break;
        case TypeClob:
            blob_segment = CORBA::string_alloc(chunkSize);
            break;
        case TypeWclob:
            blob_segment = reinterpret_cast<char*>
                    (CORBA::wstring_alloc(chunkSize));
            break;
        default:
            blob_segment = new char[chunkSize];
    }
    char* data = blob_segment;
    unsigned short actual_seg_length = 0;
    actual_length = 0;
    more = true;
    for (unsigned long i = chunkSize/USHRT_MAX; i > 0 && more;
            --i, data += actual_seg_length) {
        ISC_STATUS stat = isc_get_segment(status, &handle, &actual_seg_length
                , USHRT_MAX, data);
		if (status[0] == 1 && status[1] && status[1] != isc_segstr_eof
                && status[1] != isc_segment) {
			check_error(status, "isc_get_segment");
		}
		more = (status[1] != isc_segstr_eof);
        actual_length += actual_seg_length;
    }
    unsigned short last_length = chunkSize % USHRT_MAX;
    if (more && (last_length > 0)) {
        ISC_STATUS stat = isc_get_segment(status, &handle, &actual_seg_length
                , last_length, data);
		if (status[0] == 1 && status[1] && status[1] != isc_segstr_eof
                && status[1] != isc_segment) {
			check_error(status, "isc_get_segment");
		}
		more = (status[1] != isc_segstr_eof);
        actual_length += actual_seg_length;
    }
    if (isEntireRead) more = false; // Rely on length
    return blob_segment;
}

isc_blob_handle
InterBaseQuery::open_lob_for_reading(ISC_QUAD& blob_id) {
    isc_blob_handle handle = NULL;
	ISC_STATUS* status = handles_p_->status();
	isc_db_handle db_handle = handles_p_->db_handle();

	if (isc_open_blob2(
		status,
		&db_handle,
		current_transaction_p(),
		&handle,/* set by this function to refer to the Blob */
		&blob_id, /* Blob ID put into out_sqlda by isc_dsql_fetch() */
		0,     /* BPB length = 0; no filter will be used */
		NULL     /* NULL BPB, since no filter will be used */))
	{
		check_error(status, "isc_open_blob2");
	}
    return handle;
}
