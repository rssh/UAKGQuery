import java.io.*;
import java.util.*;

import org.omg.CORBA.*;
import org.omg.CosQuery.*;
import org.omg.CORBA.ORBPackage.*;
import org.omg.CosQueryCollection.*;

import ua.kiev.gradsoft.RC.*;
import ua.kiev.gradsoft.UAKGQuery.*;
    
public class step2
{
    private static ORB orb;

    public static void main(String args[])
    {
        try
        {
            ORB orb = ORB.init(args, props);
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
            UAKGQueryManager queryManager;
            try 
            {
                queryManager = dbcm.createQueryManager("scott", "tiger", "", "Oracle8", "");
            }
            catch(QueryManagerNotFound ex)
            {
                System.out.println("Can't find query manager.");
                return;
            }
            run_evaluate(queryManager);
            System.out.println("disconnecting.");
        }
        catch(SystemException ex)
        {
            System.out.println("SystemException" + ex);
            return;
        }
        System.out.println("Success!!!");
    }
    private static void run_evaluate(UAKGQueryEvaluator evaluator)
    {
        //static const char* query = "select TNAME, TABTYPE from tab";
        String query = "SELECT * FROM tab";
        //evaluate_records:
        RecordDescriptionHolder emptyRecordDescriptionHolder = new RecordDescriptionHolder();
        emptyRecordDescriptionHolder.value = new FieldDescription[0];
        RecordSeqHolder emptyRecordSeqHolder = new RecordSeqHolder();
        emptyRecordSeqHolder.value = new FieldValue[0][0];
        try 
        {
            RecordSeqHolder recordSeq =
                new RecordSeqHolder
                (
                    evaluator.evaluate_records
                    (
                        query, 
                        "", 
                        emptyRecordDescriptionHolder.value,
                        emptyRecordSeqHolder.value,
                        0   
                    )
                );
            System.out.println("Records:");
            System.out.println(recordSeq.value.length);
            printRecordSeq(System.out, recordSeq);
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
            System.out.println("QueryProcessingError" + ex.why);
        }
        // evlaluate_rc
        byte[] emptyOctSeq = {};
        try 
        {
            byte[] octSeq = 
                evaluator.evaluate_rc
                (
                    query,
                    "",
                    emptyRecordDescriptionHolder.value,
                    emptyOctSeq,
                    0
                );
            System.out.println("RC:");
            try 
            {
                printRC(System.out, octSeq);
            }
            catch(BadOctSeq ex)
            {
                System.out.println("Bad Octet Sequence, pos = " + ex.pos);
                System.out.println(ex.reason);
            }
        }
        catch(QueryInvalid ex)
        {
            System.out.println("QueryTypeInvalid");
        }
        catch(QueryTypeInvalid ex)
        {
            System.out.println("QueryTypeInvalid");
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
