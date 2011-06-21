
CORBA_ORB *myORB;
QueryManager_ptr queryManager_p;

void  initQueryService (int argc, char* argv[])
{
    CORBA_ORB_ptr orb = CORBA_ORB_init(argc, argv);
    myORB = (CORBA_ORB*)orb;
    CORBA_Object_var obj;
    try {
        obj = orb -> resolve_initial_references("QueryService");
    } catch(const CORBA_ORB::InvalidName&) {
        cout << " can't resolve `QueryService'" << endl;
    }
    CosQuery_QueryManager_ptr qm = CosQuery_QueryManager::_narrow(obj);
    if(CORBA_is_nil(qm)) {
        cout << "Can't narrow QM." << endl;
    }
    assert(!CORBA_is_nil(qm));
    queryManager_p = qm;
}

void receiveQuery (char * sqlText)
{
    CORBA_InterfaceDef_ptr q_type = get_query_type("CosQuery::SQL_92Query");
    if (CORBA_is_nil(q_type)) {
        cout << "can't find sql_type" << endl;
    }

    CosQuery_QueryParams_ptr qParams = createQueryParams();

    try {
        qParams->query_type_name(CORBA_string_dup("IteratorQuery"));
        qParams->result_type(CosQueryCollection__tc_Iterator);
        query_ptr =
            queryManager_ptr->create(
                     CORBA_string_dup(sqlStr),q_type,*(qParams->get()));
        CosQuery_IteratorQuery_var
            iteratorQuery = CosQuery_IteratorQuery::_narrow(query_ptr);
        query_ptr->execute(*qParams->get());
        CosQueryCollection_Iterator_ptr iterator = 
                                iteratorQuery->get_result_in_iterator();
        for (int i=0 ; iterator->more() ; i++) {
           Any_var any_record = new CORBA_Any;
           any_record = iterator->next();
           //printRecord(any_record);
         }
        iterator->destroy();
        cout << "After iterator destroy" << endl;
        query_ptr->destroy();
        cout << "After destroys" << endl;

    } catch(CosQuery_QueryInvalid ex) {
        cout << "Failed perform query: " << sqlStr << endl;
    }

}

int main(int argc, char** argv)
{
}
