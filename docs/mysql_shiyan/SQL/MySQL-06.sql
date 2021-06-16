
CREATE DATABASE mysql_shiyan;

use mysql_shiyan;

CREATE TABLE department
(
  dpt_name   CHAR(20) NOT NULL,
  people_num INT(10) DEFAULT '100',
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
  end_date   DATE DEFAULT '2020-06-01',
  of_dpt     CHAR(20) REFERENCES department(dpt_name),
  CONSTRAINT proj_pk PRIMARY KEY (proj_num,proj_name)
 );

CREATE TABLE table_1
(
l_1 INT(10) PRIMARY KEY,
l_2 INT(10),
l_3 INT(10)
 );



#INSERT INTO department(dpt_name,people_num) VALUES('部门',人数);

INSERT INTO department(dpt_name,people_num) VALUES('dpt1',110);
INSERT INTO department(dpt_name,people_num) VALUES('dpt2',120);
INSERT INTO department(dpt_name,people_num) VALUES('dpt3',100);
INSERT INTO department(dpt_name,people_num) VALUES('dpt4',150);


#INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(编号,'名字',年龄,工资,电话,'部门');

INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(01,'xiaolan',26,2500,119119,'dpt4');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(02,'xiaohong',24,2500,120120,'dpt2');
INSERT INTO employee(id,name,salary,phone,in_dpt) VALUES(03,'xiaobai',3600,019283,'dpt2');
INSERT INTO employee(id,name,salary,phone,in_dpt) VALUES(04,'xiaofu',3400,102938,'dpt3');
INSERT INTO employee(id,name,age,salary,phone,in_dpt) VALUES(05,'xiaoli',22,2800,114114,'dpt3');



#INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(编号,'工程名','开始时间','结束时间','部门名');

INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(01,'proj_a','2020-01-15','2020-01-31','dpt2');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(02,'proj_b','2020-01-15','2020-02-15','dpt1');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(03,'proj_c','2020-02-01','2020-03-01','dpt4');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(04,'proj_d','2020-02-15','2020-04-01','dpt3');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(05,'proj_e','2020-02-25','2020-03-01','dpt4');
INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(06,'proj_f','2020-02-26','2020-03-01','dpt2');


INSERT INTO project(proj_num,proj_name,start_date,end_date,of_dpt) VALUES(06,'proj_f','2020-02-26','2020-03-01','dpt2');



 