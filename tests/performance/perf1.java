import java.io.*;
import java.util.*;

import org.omg.CORBA.*;
import org.omg.CosQuery.*;
import org.omg.CORBA.ORBPackage.*;
import org.omg.CosQueryCollection.*;

import ua.kiev.grad.RC.*;
import ua.kiev.grad.CosQuery.*;
import ua.kiev.grad.CosQuery.RecordDescriptionHolder;
import ua.kiev.grad.CosQuery.RecordSeqHolder;
import ua.kiev.grad.CosQuery.FieldDescription;
    
public class perf1
{
    private static ORB orb;
    private static final int maxNRuns = 3;
    private static final long maxNRetrieved = 100;
    public static void main(String args[])
    {
        try
        {
            Properties props = System.getProperties();
            props.put("ORBClass", "com.ooc.CORBA.ORB");
            props.put("ORBSingletonClass", "com.ooc.CORBA.ORBSingleton");
            System.setProperties(props);
            orb = ORB.init(args, props);
            org.omg.CORBA.Object obj = null;
            DBConnectionManager dbcm = null;
            try
            {
                obj = orb.resolve_initial_references("UAKGQueryService");
            }
            catch(InvalidName ex)
            {
                System.out.println("UAKGQueryService service is not available");
                return;
            }
            if(obj == null)
            {
                System.out.println("UAKGQueryService object reference is invalid");
                return;
            }
            dbcm = DBConnectionManagerHelper.narrow(obj);
            if(dbcm == null)
            {
                System.out.println("Can't narrow DBConnection to correct type");
                return;
            }                                              

            UAKGQueryManager queryManager = null;
            try 
            {
                queryManager = dbcm.createQueryManager("intbill","intbill","","Oracle8","");
            }
            catch(QueryManagerNotFound ex)
            {
                System.out.println("Can't find query manager.");
                return;
            }
            run_query(queryManager);
            System.out.println("disconnecting.");
        }
        catch(SystemException ex)
        {
            System.out.println("SystemException: " + ex.getMessage());
            return;
        }
        System.out.println("Success!!!");
    }
    static long diff_ftime(Date x, Date y)
    {
        return x.getTime() - y.getTime();
        /*double x1 = difftime(x.time, y.time);
        double x2 = x.millitm - y.millitm;
        return (long)(x1*10000 + x2);  */
    }
    static void run_query(UAKGQueryManager queryManager)
    {
        String query_text = "SELECT login, host_ip FROM seances WHERE rownum < 10000";
        //evaluate_records:
        RecordDescriptionHolder emptyRecordDescriptionHolder = new RecordDescriptionHolder();
        emptyRecordDescriptionHolder.value = new FieldDescription[0];
        RecordSeqHolder emptyRecordSeqHolder = new RecordSeqHolder();
        emptyRecordSeqHolder.value = new FieldValue[0][0];

        long deltaTime[] = new long[maxNRuns];
        double avg_delta;

        Date frsTime = new Date();
        Date sndTime = new Date();
        try 
        {
            for(int nRetrieved = 0; nRetrieved < maxNRetrieved; nRetrieved++) 
            {
                avg_delta = 0;
                for(int nRuns = 0; nRuns < maxNRuns; nRuns++) 
                {
                    frsTime.setTime(System.currentTimeMillis());
                    UAKGQuery query = queryManager.create_query(query_text, 0);
                    query.prepare_query(emptyRecordDescriptionHolder.value);
                    query.execute_records(emptyRecordSeqHolder.value);
                    BooleanHolder more = new BooleanHolder(true);
                    RecordSeqHolder recordSeqHolder = new RecordSeqHolder();
                    while(more.value) 
                    {
                        recordSeqHolder.value = query.fetch_records(nRetrieved, more);
                        //printRecordSeq(cout, recordSeqHolder);
                    }
                    query.destroy();
                    sndTime.setTime(System.currentTimeMillis());
                    deltaTime[nRuns] = diff_ftime(sndTime, frsTime);      
                    //System.out.println("(i,deltat_t) = " + nRetrieved + "," + deltaTime[nRuns]);
                    avg_delta += deltaTime[nRuns];
                }
                avg_delta /= maxNRuns;
                System.out.println(nRetrieved + "," + avg_delta);
            }
        }
        catch(QueryTypeInvalid ex)
        {
            System.out.println("QueryTypeInvalid");
        }
        catch(QueryInvalid ex)
        {
            System.out.println("QueryInvalid");
        }
        catch(QueryProcessingError ex)
        {
            System.out.println("QueryProcessingError");
            System.out.println(ex.why);
        }

    }
    private static void printRecordSeq(PrintStream out, RecordSeqHolder recordSeq)
    {
        for(int row = 0; row < recordSeq.value.length; row++) 
            printRecord(out, recordSeq.value[row]);
    }
    private static void printRecord(PrintStream out, FieldValue[] record)
    {
        out.print('|');
        for(int col = 0; col < record.length; col++) {
            printField(out, record[col]);
        out.print('|');
        }
        out.print('\n');
    }
    private static void printField(PrintStream out, FieldValue fv)
    {
        if (fv.discriminator()) 
        {
            out.print("NULL");
            return;
        }
        Value v = fv.v();
        switch(v.discriminator().value()) {
            case ValueType._TypeBoolean:
                out.print((v.b()?'T':'F')); 
                break;
            case ValueType._TypeChar:
                out.print(v.c());
                break;
            case ValueType._TypeOctet:
                out.print(v.o());
                break;
            case ValueType._TypeShort:
                out.print(v.s());
                break;
            case ValueType._TypeUShort:
                out.print(v.us());
                break;
            case ValueType._TypeLong:
                out.print(v.l());
                break;
            case ValueType._TypeULong:
                out.print(v.ul());
                break;
            case ValueType._TypeFloat:
                out.print(v.f());
                break;
            case ValueType._TypeDouble:
                out.print(v.d());
                break;
            case ValueType._TypeString:
                out.print(v.str());
                break;
            case ValueType._TypeObject:
                {
                    String s = orb.object_to_string(v.obj());
                    out.print(s);
                }
                break;
            case ValueType._TypeAny:
                out.print("Any ??");
                break;
            case ValueType._TypeSmallInt:
                out.print(v.si());
                break;
            case ValueType._TypeInteger:
                out.print(v.i());
                break;
            case ValueType._TypeReal:
                out.print(v.r());
                break;
            case ValueType._TypeDoublePrecision:
                out.print(v.dp());
                break;
            case ValueType._TypeCharacter:
                out.print(v.ch());
                break;
            case ValueType._TypeDecimal:
                printDecimal(out,v.dec());
                break;
            case ValueType._TypeNumeric:
                printDecimal(out,v.n());
                break;
            case ValueType._TypeRaw:
                byte[] raw = v.raw();
                printRaw(out,raw);
                break;
            case ValueType._TypeLongRaw:
                byte[] lrawid = v.lrawid();
                printRaw(out,lrawid);
                break;
            case ValueType._TypeLongString:
                {
                    String s = v.lstr();
                    out.print(s);
                }
                break;
            /*case ValueType._TypeWString:
                {
                    String s = v.wstr();
                    out.print(s);
                }
                break;*/
            default:
                out.print("UnknownFieldType");
        }
    }
    private static void printDecimal(PrintStream out, Decimal dec)
    {
        out.print("prec: " + dec.precision + ", scale:" + dec.scale + ", value:");
        printRaw(out, dec.value);
    }

    private static void printRaw(PrintStream out, byte[] octSeq)
    {
        for(int i = 0; i < octSeq.length; i++)
        {
            int x = (int)(octSeq[i]);
            out.print('[' + x + ']');
        }
    }
    private static void printRC(PrintStream out, byte[] octSeq) 
        throws BadOctSeq
    {
        RCHeader header = new RCHeader((byte)0,0,0);
        RCHeaderHolder headerHolder = new RCHeaderHolder(header);
        IntHolder pos = new IntHolder(0);
        ReaderHolder reader = new ReaderHolder();
        RCReader.readHeader(header, pos, octSeq);
        out.print("Header: nFields = " + header.nFields + '\n');
        out.print("Header: nRecords = " + header.nRecords + '\n');
        for(int nRecord = 0; nRecord < header.nRecords; nRecord++)
        {
            for(int nField = 0; nField < header.nFields; nField++)
            {
                FieldValue fv = RCReader.readField(pos, octSeq);
                printField(out, fv);
                out.print('|');
            }
            out.print('\n');
        }
    }
}
