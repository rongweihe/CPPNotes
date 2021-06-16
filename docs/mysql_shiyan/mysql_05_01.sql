CREATE DATABASE test;

CREATE DATABASE mysql_shiyan;

use mysql_shiyan;

CREATE TABLE department
(
  dpt_name   CHAR(20) NOT NULL,
  people_num INT(10) DEFAULT '10',
  CONSTRAINT dpt_pk PRIMARY KEY (dpt_name)
 );

CREATE TABLE employee
(
  id      INT(10) PRIMARY KEY,
  name    CHAR(20),
  age     INT(10),
  salary  INT(10) NOT NULL,
  phone   INT(12) NOT NULL,
  in_dpt  CHAR(20) NOT NULL,
  UNIQUE  (phone),
  CONSTRAINT emp_fk FOREIGN KEY (in_dpt) REFERENCES department(dpt_name)
 );
 
CREATE TABLE project
(
  proj_num   INT(10) NOT NULL,
  proj_name  CHAR(20) NOT NULL,
  start_date DATE NOT NULL,
  end_date   DATE DEFAULT '2020-05-01',
  of_dpt     CHAR(20) REFERENCES department(dpt_name),
  CONSTRAINT proj_pk PRIMARY KEY (proj_num,proj_name)
 );

CREATE TABLE table_1
(
id INT(10) PRIMARY KEY,
l2 INT(10),
l3 INT(10)
);


INSERT INTO department(dpt_name,people_num) VALUES('dpt1',100);
INSERT INTO department(dpt_name,people_num) VALUES('dpt2',200);
INSERT INTO department(dpt_name,people_num) VALUES('dpt3',300);
INSERT INTO department(dpt_name,people_num) VALUES('dpt4',400);


#INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(编号,'名字',年龄,工资,电话,'部门');

INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(01,'xiaobai',24,5000,119119,'dpt4');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(02,'xiaolan',25,5500,120120,'dpt2');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(03,'xiaofu',26,6000,114114,'dpt3');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(04,'xiaohong',35,65000,100861,'dpt1');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(05,'xiaowei',22,7000,100101,'dpt2');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(06,'xiaoba',26,7500,123456,'dpt1');


#INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(编号,'工程名','开始时间','结束时间','部门名');

INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(01,'proj_a','2020-01-15','2020-01-31','dpt2');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(02,'proj_b','2020-01-15','2020-02-15','dpt1');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(03,'proj_c','2020-02-01','2020-03-01','dpt4');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(04,'proj_d','2020-02-15','2020-04-01','dpt3');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(05,'proj_e','2020-02-25','2020-03-01','dpt4');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(06,'proj_f','2020-02-26','2020-03-01','dpt2');


#INSERT INTO table_1 VALUES(01,11,12);

INSERT INTO table_1 VALUES(02,45,99);
INSERT INTO table_1 VALUES(03,50,57);
INSERT INTO table_1 VALUES(04,36,60);
INSERT INTO table_1 VALUES(05,40,32);
INSERT INTO table_1 VALUES(06,90,42);

CREATE TABLE test_kafka
(
  id INT(10) PRIMARY KEY,
  site CHAR(20) NOT NULL,
  host CHAR(50) NOT NULL,
  apid CHAR(20) NOT NULL,
  name CHAR(20) NOT NULL,
  package_name CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
  site CHAR(20) NOT NULL,
);


INSERT INTO test_kafka(id,det_name_int) VALUES(1,'沪江开心词场');
