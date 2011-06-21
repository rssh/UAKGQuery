package ua.kiev.grad.RC;

/*
 * Decoder
 * (C) Copyright 2000 Alexander Bourla <AlexB@grad.kiev.ua>
 * $Source: /home/rssh/cvs-dump/cvsroot/COS/CosQuery.1/jsrc/ua/kiev/grad/RC/RCReader.java,v $
 * $Id: RCReader.java,v 1.7 2000-04-03 11:48:03 alexb Exp $
 * Last modified by: $Author: alexb $
 */

import org.omg.CosQueryCollection.*;
import java.util.*;

public class RCReader // implements Reader
{
    public static void
    readHeader(RCHeader header,
               org.omg.CORBA.IntHolder pos,
               byte[] octSeq)
        throws BadOctSeq
    {
        pos.value = 0;
        header.version = readOctet(pos, octSeq);
        header.nRecords = readLong(pos, octSeq);
        header.nFields = readOctet(pos, octSeq);
    }

    public static org.omg.CosQueryCollection.FieldValue[]
    readRecord(org.omg.CORBA.IntHolder pos,
               byte[] octSeq)
        throws BadOctSeq
    {
        byte o = readOctet(pos, octSeq);
        int n = (o < 0)? (o + 0x100) : o;
        FieldValue[] record = new FieldValue[n];
        for(byte i = 0; i < n; i++)
            record[i] = readField(pos, octSeq);
        return record;
    }

    public static org.omg.CosQueryCollection.FieldValue[][]
    readRecordSeq(org.omg.CORBA.IntHolder pos,
                  byte[] octSeq)
        throws BadOctSeq
    {
        Vector recordSeqV = new Vector();
        while(pos.value < octSeq.length) 
        {
              FieldValue[] record = readRecord(pos, octSeq);
              recordSeqV.addElement(record);
        }
        FieldValue[][] recordSeq = new FieldValue[recordSeqV.size()][];
        recordSeqV.copyInto(recordSeq);
        return recordSeq;
    }

    public static org.omg.CosQueryCollection.FieldValue
    readField(org.omg.CORBA.IntHolder pos,
              byte[] octSeq)
        throws BadOctSeq
    {
        FieldValue fv = new FieldValue();
        readField_inout(fv, pos, octSeq);
        return fv;
    }

    public static void 
    readField_inout(org.omg.CosQueryCollection.FieldValue fv,
                    org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
            throws BadOctSeq
    {
        byte fieldType = readOctet(pos, octSeq);
        org.omg.CosQueryCollection.Value v = new org.omg.CosQueryCollection.Value();
        switch(fieldType) 
        {
            case 0x00:
                fv._default();
                return;
            case 0x01:
                v.b(readBoolean(pos, octSeq));
                break;
            case 0x02:
                v.c(readChar(pos, octSeq));
                break;
            case 0x03:
                v.o(readOctet(pos, octSeq)); 
                break;
            case 0x04:
                v.s(readShort(pos, octSeq));
                break;
            case 0x05:
                v.us(readUShort(pos, octSeq));
                break;
            case 0x06:
                v.l(readLong(pos, octSeq));
                break;
            case 0x07:
                v.ul(readULong(pos, octSeq));
                break;
            case 0x08:
                v.r(readFloat(pos, octSeq));
                break;
            case 0x09:
                v.d(readDouble(pos, octSeq));
                break;
            case 0x0A:
                v.str(readString(pos, octSeq));
                break;
            case 0x0B:
                v.obj(readObject(pos, octSeq));
                break;
            case 0x0C:
                v.a(readAny(pos, octSeq));
                break; 
            case 0x0D:
                v.si(readShort(pos, octSeq));
                break;
            case 0x0E:
                v.i(readLong(pos, octSeq));
                break;
            case 0x0F:
                v.dec(readDecimal(pos, octSeq));
                break;
            case 0x10:
                v.n(readDecimal(pos, octSeq));
                break;
            case 0x11:
                v.raw(readRaw(pos, octSeq));
                break;
            case 0x12:
                /*v.long_raw*/v.lrawid(readLongRaw(pos, octSeq));
                break;
            case 0x13:
                /*v.long_string*/v.lstr(readString(pos, octSeq));
                break;
            case 0x14:
                throw new BadOctSeq(pos.value, "WStrings are not implemented");
            default:
                throw new BadOctSeq(pos.value, "Unknown type");
       }
       fv.v(v);
    }
    
    /**
     * return true and skip null value, if return was null, otherwise
     * return false and not touch pos.
     **/
    public static boolean
    nextFieldIsNull(org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq
    {
        if ((char)octSeq[pos.value] == '\0')
        {
            pos.value++;
            return true;
        }
        return false;
    }

    public static org.omg.CosQueryCollection.ValueType
    nextFieldType(org.omg.CORBA.IntHolder pos,
                  byte[] octSeq)
        throws BadOctSeq, FieldValueIsNull
    {
        ValueType retval;
        switch(octSeq[pos.value]) 
        {
            case 0x00:
                  throw new FieldValueIsNull();
             case 0x01:
                  retval = ValueType.TypeBoolean;
                  break;
             case 0x02:
                  retval = ValueType.TypeChar;          
                  break;
             case 0x03:
                  retval = ValueType.TypeOctet;          
                  break;
             case 0x04:
                  retval = ValueType.TypeShort;          
                  break;
             case 0x05:
                  retval = ValueType.TypeUShort;          
                  break;
             case 0x06:
                  retval = ValueType.TypeLong;          
                  break;
             case 0x07:
                  retval = ValueType.TypeULong;          
                  break;
             case 0x08:
                  retval = ValueType.TypeFloat;          
                  break;
             case 0x09:
                  retval = ValueType.TypeDouble;          
                  break;
             case 0x0A:
                  retval = ValueType.TypeString;
                  break;
             case 0x0B:
                  retval = ValueType.TypeObject;
                  break;
             case 0x0C:
                  retval = ValueType.TypeAny;
                  break;
             case 0x0D:
                  retval = ValueType.TypeSmallInt;
                  break;
             case 0x0E:
                  retval = ValueType.TypeInteger;
                  break;
             case 0x0F:
                  retval = ValueType.TypeDecimal;
                  break;
             case 0x10:
                  retval = ValueType.TypeNumeric;
                  break;
             case 0x11:
                  retval = ValueType.TypeRaw;
                  break;
             case 0x12:
                  retval = ValueType.TypeLongRaw;
                  break;
             case 0x13:
                  retval = ValueType.TypeLongString;
                  break;
             case 0x14:
                  retval = ValueType.TypeLongString;
                  break;
             default:
                  throw new BadOctSeq(pos.value, "Unknown type of field");
          }
          pos.value++;
          return retval;
    }

    public static void checkNullAndFieldType
    (
        org.omg.CosQueryCollection.ValueType vt,
        org.omg.CORBA.IntHolder pos,
        byte[] octSeq
    )
    throws BadOctSeq, 
           FieldValueIsNull
    {
        if (!nextFieldType(pos, octSeq).equals(vt)) 
            throw new BadOctSeq(pos.value, "We want to read other type");
    }

    public static boolean
    readBooleanField(org.omg.CORBA.IntHolder pos,
                     byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeBoolean, pos, octSeq);
        return readBoolean(pos, octSeq);
    }

    public static void
    readBooleanField_inout(org.omg.CORBA.BooleanHolder value,
                           org.omg.CORBA.IntHolder pos,
                           byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        value.value = readBooleanField(pos, octSeq);
    }

    public static char
    readCharField(org.omg.CORBA.IntHolder pos,
                  byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeChar, pos, octSeq);
        return readChar(pos, octSeq);
    }

    public static void
    readCharField_inout(org.omg.CORBA.CharHolder value,
                        org.omg.CORBA.IntHolder pos,
                        byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        value.value = readCharField(pos, octSeq);
    }

    public static byte
    readOctetField(org.omg.CORBA.IntHolder pos,
                   byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeOctet, pos, octSeq);
        return readOctet(pos, octSeq);
    }

    public static short
    readShortField(org.omg.CORBA.IntHolder pos,
                   byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeShort, pos, octSeq);
        return readShort(pos, octSeq);
    }

    public static short
    readUShortField(org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeUShort, pos, octSeq);
        return readShort(pos, octSeq);
    }

    public static int
    readLongField(org.omg.CORBA.IntHolder pos,
                  byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeLong, pos, octSeq);
        return readLong(pos, octSeq);
    }

    public static int
    readULongField(org.omg.CORBA.IntHolder pos,
                   byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeULong, pos, octSeq);
        return readULong(pos, octSeq);
    }

    public static float
    readFloatField(org.omg.CORBA.IntHolder pos,
                   byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeFloat, pos, octSeq);
        return readFloat(pos, octSeq);
    }

    public static double
    readDoubleField(org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeDouble, pos, octSeq);
        return readDouble(pos, octSeq);
    }

    public static String
    readStringField(org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeString, pos, octSeq);
        return readString(pos, octSeq);
    }

    public static org.omg.CORBA.Object
    readObjectField(org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeObject, pos, octSeq);
        return readObject(pos, octSeq);
    }

    public static org.omg.CORBA.Any
    readAnyField(org.omg.CORBA.IntHolder pos,
                 byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeAny, pos, octSeq);
        return readAny(pos, octSeq);
    }

    public static org.omg.CosQueryCollection.Decimal
    readDecimalField(org.omg.CORBA.IntHolder pos,
                     byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeDecimal, pos, octSeq);
        return readDecimal(pos, octSeq);
    }

    public static org.omg.CosQueryCollection.Decimal
    readNumericField(org.omg.CORBA.IntHolder pos,
                     byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeNumeric, pos, octSeq);
        return readDecimal(pos, octSeq);
    }

    public static byte[]
    readRawField(org.omg.CORBA.IntHolder pos,
                 byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeRaw, pos, octSeq);
        return readRaw(pos, octSeq);
    }

    public static void
    readRawField_inout(OctSeqHolder value,
                       org.omg.CORBA.IntHolder pos,
                       byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeLongRaw, pos, octSeq);
        value.value = readLongRaw(pos, octSeq);
    }

    public static byte[]
    readLongRawField(org.omg.CORBA.IntHolder pos,
                     byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeLongRaw, pos, octSeq);
        return readLongRaw(pos, octSeq);
    }

    public static void
    readLongRawField_inout(OctSeqHolder value,
                           org.omg.CORBA.IntHolder pos,
                           byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeRaw, pos, octSeq);
        int rawLength = readULong(pos, octSeq); 
        System.arraycopy(octSeq, pos.value, value, 0, rawLength);
        pos.value += rawLength;
    }

    public static String
    readLongStringField(org.omg.CORBA.IntHolder pos,
                        byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeRaw, pos, octSeq);
        return readString(pos, octSeq); 
    }

    public static void readString_inout(org.omg.CORBA.StringHolder value,
                              org.omg.CORBA.IntHolder pos,
                              byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        value.value = readString(pos, octSeq); 
    }
    
    public static void
    readLongStringField_inout(org.omg.CORBA.StringHolder value,
                              org.omg.CORBA.IntHolder pos,
                              byte[] octSeq)
        throws BadOctSeq,
               FieldValueIsNull
    {
        checkNullAndFieldType(ValueType.TypeLongString, pos, octSeq);
        readString_inout(value, pos, octSeq);
    }

    public static boolean
    readBoolean(org.omg.CORBA.IntHolder pos,
                byte[] octSeq)
        throws BadOctSeq
    {
        return (octSeq[pos.value++] != 0);
    }

    public static void
    readBoolean_inout(org.omg.CORBA.BooleanHolder value,
                      org.omg.CORBA.IntHolder pos,
                      byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readBoolean(pos, octSeq);
    }

    public static char
    readChar(org.omg.CORBA.IntHolder pos,
             byte[] octSeq)
        throws BadOctSeq
    {
        return (char)octSeq[pos.value++];
    }

    public static void
    readChar_inout(org.omg.CORBA.CharHolder value,
                   org.omg.CORBA.IntHolder pos,
                   byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readChar(pos, octSeq);
    }

    public static byte
    readOctet(org.omg.CORBA.IntHolder pos,
              byte[] octSeq)
        throws BadOctSeq
    {
      return octSeq[pos.value++];
    }

    public static void
    readOctet_inout(org.omg.CORBA.ByteHolder value,
                    org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readOctet(pos, octSeq);
    }

    public static short
    readShort(org.omg.CORBA.IntHolder pos,
              byte[] octSeq)
        throws BadOctSeq
    {
        return readUShort(pos, octSeq);
    }

    public static void
    readShort_inout(org.omg.CORBA.ShortHolder value,
                    org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readShort(pos, octSeq);
    }

    public static short
    readUShort(org.omg.CORBA.IntHolder pos,
               byte[] octSeq)
        throws BadOctSeq
    {
        byte o;
        o = octSeq[pos.value];
        short i1 = (short)((o < 0)? (o + 0x100) : o);
        o = octSeq[pos.value + 1];
        short i2 = (short)((o < 0)? (o + 0x100) : o);
        short v = (short)((i1 << 8) + i2);
        pos.value += 2;
        return v;
    }

    public static void
    readUShort_inout(org.omg.CORBA.ShortHolder value,
                     org.omg.CORBA.IntHolder pos,
                     byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readShort(pos, octSeq);
    }

    public static int
    readLong(org.omg.CORBA.IntHolder pos,
             byte[] octSeq)
        throws BadOctSeq
    {
        return readULong(pos, octSeq);
    }

    public static void
    readLong_inout(org.omg.CORBA.IntHolder value,
                   org.omg.CORBA.IntHolder pos,
                   byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readULong(pos, octSeq);
    }

    public static int
    readULong(org.omg.CORBA.IntHolder pos,
              byte[] octSeq)
        throws BadOctSeq
    {
        int v = 0;
        for (byte i = 0; i < 4; i++) 
        {
            byte o = octSeq[pos.value + i];
            short b = (short)((o < 0)? (o + 0x100) : o);
            v = (int)(v + (b << (3-i)*8));
        }
        pos.value += 4;
        return v;
    }

    public static void//was as an int in interface
    readULong_inout(org.omg.CORBA.IntHolder value,
                    org.omg.CORBA.IntHolder pos,
                    byte[] octSeq)
        throws BadOctSeq
    {
        value.value = readULong(pos, octSeq);
    }

    public static float
    readFloat(org.omg.CORBA.IntHolder pos,
              byte[] octSeq)
        throws BadOctSeq
    {
        int tmp_retval = readULong(pos, octSeq);
        return Float.intBitsToFloat(tmp_retval);
    }

    public static double
    readDouble(org.omg.CORBA.IntHolder pos,
               byte[] octSeq)
        throws BadOctSeq
    {
        long tmp_retval = readULong(pos, octSeq);
        return Double.longBitsToDouble(tmp_retval);
    }

    public static String
    readString(org.omg.CORBA.IntHolder pos,
               byte[] octSeq)
        throws BadOctSeq
    {
        int len = readULong(pos, octSeq);
        if (len > octSeq.length) throw new BadOctSeq(pos.value, "There is not enough octets for String Field");
        byte[] retval = new byte[len];
        System.arraycopy(octSeq, pos.value, retval, 0, len);
        pos.value += len;
        return new String(retval);
    }

    public static org.omg.CORBA.Object
    readObject(org.omg.CORBA.IntHolder pos,
               byte[] octSeq)
        throws BadOctSeq
    {
        String s = readString(pos, octSeq);
        org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init();
        return orb.string_to_object(s);
    }

    public static org.omg.CORBA.Any
    readAny(org.omg.CORBA.IntHolder pos,
            byte[] octSeq)
        throws BadOctSeq
    {
        //org.omg.CORBA.AnyHolder a = new org.omg.CORBA.AnyHolder();
        //return a.value; //??????
        throw new BadOctSeq(pos.value, "Type Any is not supported");
    }

    public static org.omg.CosQueryCollection.Decimal
    readDecimal(org.omg.CORBA.IntHolder pos,
                byte[] octSeq)
        throws BadOctSeq
    {
        org.omg.CosQueryCollection.Decimal dec = new Decimal();
        dec.precision = readLong(pos, octSeq);
        dec.scale = readLong(pos, octSeq);
        dec.value = readRaw(pos, octSeq);
        return dec;
    }

    public static byte[]
    readRaw(org.omg.CORBA.IntHolder pos,
            byte[] octSeq)
        throws BadOctSeq
    {
        int rawLength = readULong(pos, octSeq);
        byte[] retval = new byte[rawLength];
        System.arraycopy(octSeq, pos.value, retval, 0, rawLength);
        pos.value += rawLength;
        return retval;
    }

    public static byte[]
    readLongRaw(org.omg.CORBA.IntHolder pos,
                byte[] octSeq)
        throws BadOctSeq
    {
        return readRaw(pos, octSeq);
    }
}
