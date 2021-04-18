

# 1、查找employees里最晚入职员工的所有信息

建表语句如下:

```sql
CREATE TABLE `employees` (
`emp_no` int(11) NOT NULL, 
`birth_date` date NOT NULL,
`first_name` varchar(14) NOT NULL,
`last_name` varchar(16) NOT NULL,
`gender` char(1) NOT NULL,
`hire_date` date NOT NULL,
PRIMARY KEY (`emp_no`));
```

参考答案：

如果是查询该员工的入职时间（去重），可以这么写

```sql
SELECT DISTINCT
    hire_date AS last_hire_date
FROM
    employees
ORDER BY hire_date DESC
LIMIT 1 OFFSET 0
```

输出：1994-09-15。

如果是查找该员工的所有信息，需要这么写：

- 知识点
  - ORDER BY 根据指定的列对结果集进行排序，默认按照升序，降序 ORDER BY DESC
  - LIMIT(m, n) 从第 m + 1 行开始取 n 条记录
- 最晚员工自然是 hire_data，最晚可以用排序 ORDER BY DESC 降序来得到，然后是获取第一条记录，这样理论上是有 bug 的，因为 hire_data 可能有多个相同的记录

```sql
SELECT * FROM employees order by hire_date desc limit 0,1
```

如果需要（去重），需要这么写：

```sql
SELECT * FROM employees WHERE hire_date = (SELECT MAX(hire_date) FROM employees)
```

