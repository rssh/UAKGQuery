
REM 1013
CREATE SEQUENCE t1013_seq MINVALUE 1 ;

REM 1015
CREATE TABLE t1015 (
 ID NUMBER(10),
 numeric NUMBER(19,9)
);

REM 1560
CREATE TABLE BLOB_TEST (
 data BLOB
);

REM 1667
CREATE TABLE t1667 (
 A VARCHAR(4 CHAR)
);

REM 1685

CREATE TABLE t1685 (
 ID NUMBER(10),
 NAME  VARCHAR2(30)
);

CREATE SEQUENCE t1685_seq MINVALUE 1 NOCYCLE;

CREATE PROCEDURE t1685_proc(ID_ OUT NUMBER, NAME_ VARCHAR2)
AS
BEGIN
  SELECT t1685_seq.nextval into ID_  FROM dual;
  insert into t1685(id,name) values(ID_,NAME_);
END;
/

show errors procedure t1685_proc;



