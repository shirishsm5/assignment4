# Assignment 4 – Processing Fixed Length Record Data with Threads

Welcome to your forth homework assignment.  

**NOTE:** The GitHub due date is the CUTOFF time and **not** the actual due date. See Canvas for the actual due date.

**Assignment Purpose and Learning Outcomes:**
- Understanding Fixed Length Record Data files and how to parse them
- Understanding error handling
- Processing large quantities of data
- Summarizing data based on specific fields and criteria
- Understanding and using Vectors (arrays of pointers)
- Comprehensive use of threads and understanding the timings
- Understanding of Race Conditions and preventing data corruption 

**Let me remind you of the general expectations for all projects:** 
- All projects are done in C in the Ubuntu Linux Virtual Machine.
- Code must be neat, with proper and consistent indentation and well documented. 
- Keep line to around 80 characters per line, and not line greater than 100 characters.
- Comments must describe the logic and reason for the code and not repeat the code.  
- Variables must have meaningful names and be in a consistent format (I do not care if you use camelCase or under_scores in variables but be consistent.
- You must use `make` to compile the program.
- You must use `make run` (sometimes with RUNOPTIONS) to execute the program.
- In addition, each file must have the standard header as defined below.
- All source files and writeup must be in the main branch of the github.
  
All primary filenames should be `<lastname>_<firstname>_HW<#>_<component>.<proper extension>`, except Makefile.

Each .c and .h file must have a standard header as defined below.  Make sure to put in your section number (replace the #), your name, your student ID, a proper project name, GitHub name, filename, and description of the project.  Do not alter the number of asterisks and ensure that the header starts on line 1 of the file.

```
/**************************************************************
* Class::  CSC-415-0# Spring 2025
* Name::
* Student ID::
* GitHub-Name::
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: <name of this file>
*
* Description::
*
**************************************************************/
```
This is an INDIVIDUAL assignment.  You can (and should) work in groups to research how to do the assignment, but each person should code their own version and make their own submission.

This assignment is in three specific parts:

1) Being able to open and correctly parse a Header and Data file
2) Being able to process data in order to summarize and visualize data
3) Being able to do these tasks in multiple threads

For this assignment, all file operations must be done using only the **Linux** file functions **open**, **read**, **pread**, **lseek**, **close**, **stat**, or **fstat**.  No other file operations are allowed and not all those listed need to be used.

**For part 1 - Fixed Length Record Data File Processing**

A Fixed Length Record Data file is a binary file where columnar (field) data is stored in fixed width fields.  The sum of the widths of each field make up the length of the total record.  All records are the same length, and each field within the record as a fixed position and width.
To determine the record length and to handle columns you will have to read the header file (called "header.txt") that contains the width of the field and the name of the field.  A portion of the file looks like this:

```
10: cad_number
10: dup_cad_number
15: pd_incident_report
25: received_datetime
...
35: analysis_neighborhood
15: police_district
...
```
The sum of all the field lengths is the size of each record in the database.  You can use the function **atoi** to convert ascii strings to integer numbers.  Think about what information you actually need for each field you need.

The location in the file of any record (n) is simply (n-1) * recordlength.  This makes it easy to assign different sets of records to different threads (more on this in part 3).  It also means that the number of records in the file is simply the total file size / recordlength.

The data file is composed on multiple records each records with the fields specified in the header file.  The contents of each field is an ASCII string (null terminated).  Specifically, datetime fields such as **received_datetime** are stored in an ascii format of MM/DD/YYYY HH:MM:SS AM/PM, example: "02/26/2025 10:09:15 PM".  Since you will be subtracting two different datetime fields you will want to convert it into something you can manipulate.  This could be a total number of seconds from a base time, it can be a structure containing integers for each of the fields (i.e. year, month, day, hour, minute, second) or any other way you want to manipulate the values so you can do the subtraction of one datetime from another.



**For part 2 - Data Processing**

For this part you will build a data structure (of your choice) that keeps tract of every event type (specifically the field `call_type_final_desc`, unless that field is blank then use the `call_type_original_desc` field).  You will keep with each event type the total number of calls, the values of the dispatch_datetime - received_datetime, the onscene_datetime - enroute_datetime, and the onscene_datetime - received_datetime  You will also selectively keep track of those times for either 2 neighborhoods or police districts.  Specifically 2 values from the `analysis_neighborhood` field or the `police_district` field depending on the command line parameters.  You will also calculate a Total section of the entire city regardless of the neighborhood or police district.  You will also have a total for the entire city and each of the neighborhoods/districts regardless of call type.
From the data in your data structure you will need to calculate statistics which include:

- Maintain the minimum and maximum values
- Calculate the Median (the middle number of the number set)
- Calculate the Q1 and Q3 values (which are the middle number of the lower half and the middle number of the upper half)
- Calculate the Interquartile Range (IQR) (Q3 - Q1)
- Calculate the Lower bound (the greater of the minimum value or Q1 - 1.5 * IQR)
- Calculate the Upper bound (the lesser of the maximum value or Q3 + 1.5 * IQR)
- Calculate the mean and the standard deviation
- Values below the lower bound are considered outliers as are those above the upper bound


The output should be well formatted and columnar with clear headings.   This is a short example (having all time differences in seconds) of what the output can look like:

```
Financial District/South Beach   -  OnScene Time - Recevied Time

       Call Type        | count  | min |  LB  | Q1 | med |  mean  |  Q3 | UB  | max  | IQR | stddev
                   TOTAL| 180014 |    2|    2|  547| 1253| 4888.61| 3544| 8039|358010| 2997|13400.84 
            PASSING CALL|      2 | 1196| 1196| 1196| 1204| 1204.00| 1212| 1212|  1212|   16|    8.00 
     TRAF VIOLATION CITE|   4043 |   38|   38| 1992| 3926|11493.94| 8608|18532| 95212| 6616|18869.26 
              TRESPASSER|  15718 |    2|    2| 1028| 2455| 6475.15| 5925|13270|157772| 4897|12509.35 
               TOW TRUCK|   2098 |  237|  237| 1591| 2452| 3439.97| 3893| 7346| 96987| 2302| 5101.32 
          MEET W/OFFICER|    389 |   47|   47|  276|  543| 1934.59| 1239| 2683| 44121|  963| 6226.56 
         FIGHT NO WEAPON|  15881 |   25|   25|  486|  926| 2730.93| 2237| 4863|131587| 1751| 7206.98 
       SUSPICIOUS PERSON|   7288 |    5|    5|  953| 2119| 5452.57| 5204|11580|102741| 4251|10619.81 
        WELL BEING CHECK|  11369 |   31|   31|  516|  929| 2512.18| 2011| 4253| 79871| 1495| 6595.46 
      HOMELESS COMPLAINT|   8227 |    6|    6| 1671| 4115|12422.25| 9586|21458|355637| 7915|27710.22 
              AIDED CASE|     89 |    5|    5|  302|  637| 2715.80| 1556| 3437| 59646| 1254| 8083.75 
      TRAF VIOLATION TOW|    806 |   84|   84| 1495| 2399| 4678.68| 4382| 8712| 74785| 2887| 9027.51 
             PETTY THEFT|   7606 |    2|    2|  951| 2927| 7766.05| 7580|17523|146296| 6629|14412.50 
           AUDIBLE ALARM|  17834 |   62|   62|  536| 1045| 2529.66| 2199| 4693|139390| 1663| 6600.01 
            TRAFFIC STOP|      2 |   94|   94|   94|  422|  422.50|  751|  751|   751|  657|  328.50 
          PERSON W/KNIFE|   2361 |   32|   32|  272|  405|  773.45|  604| 1102| 46208|  332| 2913.69 
             GRAND THEFT|   1462 |   34|   34|  791| 2788| 7709.38| 8024|18873|101129| 7233|14236.61 
          NOISE NUISANCE|   4518 |   73|   73| 1848| 4875|13092.30|12243|27835|149037|10395|19582.73 
      AUTO BOOST / STRIP|   3530 |   18|   18|  452|  867| 5618.70| 3395| 7809|145421| 2943|13829.78 
    MEET W/CITY EMPLOYEE|   5538 |    8|    8|  365|  649| 1551.58| 1281| 2655|130117|  916| 4784.13 
                 ROBBERY|    686 |   24|   24|  278|  415| 1743.37|  689| 1305| 85582|  411| 7366.81 
          COMPLAINT UNKN|  10649 |   55|   55|  772| 1492| 2666.33| 2901| 6094| 72427| 2129| 5144.31 
             PANIC ALARM|    724 |   76|   76|  281|  410|  722.73|  588| 1048| 44043|  307| 3225.04 
     SILENT HOLDUP ALARM|   1334 |   60|   60|  245|  356|  463.70|  534|  967| 43383|  289| 1215.28 
     
     
     
Tenderloin   -  OnScene Time - Recevied Time

       Call Type        | count  | min |  LB  | Q1 | med |  mean  |  Q3 | UB  | max  | IQR | stddev
                   TOTAL| 263904 |    2|    2|  539| 1665| 7141.25| 5439|12789|355918| 4900|17159.63 
       AGG ASSAULT / ADW|   2314 |   13|   13|  255|  385| 1666.57|  634| 1202|182468|  379| 7821.41 
       SUSPICIOUS PERSON|  32607 |    2|    2| 1420| 3628| 8770.28| 8336|18710|347295| 6916|15282.97 
      SUSPICIOUS VEHICLE|   2709 |    4|    4| 1281| 3362| 9158.39| 7905|17841|136231| 6624|15943.88 
            PASSING CALL|      2 |  186|  186|  186|  728|  728.50| 1271| 1271|  1271| 1085|  542.50 
             PANIC ALARM|    516 |   58|   58|  257|  399|  924.59|  582| 1069| 44396|  325| 4282.02 
        BLOCKED SIDEWALK|    587 |    7|    7|  639| 3110|14551.58|10363|24949|272768| 9724|28707.47 
              AIDED CASE|    121 |    2|    2|  153|  283| 1295.12|  557| 1163| 55003|  404| 5621.29 
            TRAFFIC STOP|      0 |    0|    0|    0|    0|    0.00|    0|    0|     0|    0|    0.00 
      HOMELESS COMPLAINT|  11484 |    5|    5| 1930| 5113|17173.76|12335|27942|355918|10405|37718.54 
          NOISE NUISANCE|  22419 |   20|   20| 1723| 4630|13714.53|12306|28180|341222|10583|20475.94 
       ASSAULT / BATTERY|  11392 |   16|   16|  300|  477| 2822.86|  904| 1810|176904|  604|10138.53 
        WELL BEING CHECK|  22355 |   17|   17|  550| 1189| 4050.29| 3411| 7702|145477| 2861| 9590.05 
    MEET W/CITY EMPLOYEE|   8088 |    3|    3|  291|  574| 1848.85| 1222| 2618|134611|  931| 6273.76 
         FIGHT NO WEAPON|  28074 |    5|    5|  427|  970| 4159.53| 3141| 7212|261146| 2714|10830.75 
     TRAF VIOLATION CITE|   3723 |   42|   42| 1638| 2980| 8214.91| 6275|13230|121512| 4637|15350.55 
    THREATS / HARASSMENT|   6790 |    3|    3|  831| 2684| 9687.32| 7589|17726|187062| 6758|19014.82 

```

So there should be 9 "pages" of data output
- Total   -  Dispatch Time - Recevied Time
- Total   -  OnScene Time - Enroute Time
- Total   -  OnScene Time - Recevied Time
- Neighborhood/District-1   -  Dispatch Time - Recevied Time
- Neighborhood/District-1   -  OnScene Time - Enroute Time
- Neighborhood/District-1   -  OnScene Time - Recevied Time
- Neighborhood/District-2   -  Dispatch Time - Recevied Time
- Neighborhood/District-2   -  OnScene Time - Enroute Time
- Neighborhood/District-2   -  OnScene Time - Recevied Time

You my use any data structure you like to hold the data as it is being processed, but it must be global (i.e. each thread will be adding directly to it and not merging at the end) and you have to write the data structure code yourself, not using libraries. (Note: using string libraries for copying and comparing are okay). In addition since standard deviation requires taking the square root of the sum of the squares you can #include <math.h> and use the `sqrt ()` function.

**For part 3 - Threading**
The catch, it is to do all this using threads. Each thread will manage a proportional number of the records of the file and process it, returning it’s results tallied to shared memory and then the main will print the results as specified above. Remember that this assignment will be using the pthread functions.  
If you are running with the Law500K.dat file, and you specify 2 threads, each thread will process 250,000 records, if 4 threads then each will process 125,000 records.  Since read is an expensive operation, you could have each thread read in a "group" of records at a time (say 2000 at a time, to reduce the number of callls to read).

We want to determine how long the program takes using different number of threads (separate runs).  

To Do So:
#include <time.h> in your code and in main, include the code below in your main.  This will display how much time your program takes.  Your submission must include a run with 1 thread, 2 threads, 4 threads, and 8 threads.  How do the times compare?

```
    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************

    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish


    // ***TO DO *** Display Data

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************

```
There is a template main with this code provided. (Don't forget to rename it to the naming conventions).  Do not modify in any way the blocks that start and end the timers (including the asterisks).  Make sure that other than initialization code and cleanup code that your code is between these blocks where the `TO DO`'s are.


**Command Line Parameters**
```
<program> <data filename> <header filename> <threads> <subfield> <subfield value 1> <subfield value 2>
make run RUNOPTIONS="Law500K.dat header.txt 4 police_district BAYVIEW MISSION"
make run RUNOPTIONS="Law500K.dat header.txt 2 analysis_neighborhood 'Russian Hill' 'Outer Richmond'"
```

data filename:  is the name of the data set. One of the following (each of these are links to the file):
- [Law5K.dat](https://sfsu.box.com/s/y8k4cpmi5kw40329uft3jp7bywwd57qb)
- [Law10K.dat](https://sfsu.box.com/s/l9k8sxomxmx410scamfvq7tmoxwqjiq3)
- [Law50K.dat](https://sfsu.box.com/s/6nllok30dbmv2czlgllsk6u20k53p1hh)
- [Law100K.dat](https://sfsu.box.com/s/j5n7lzuz8aumy0ta1hoizzpgstgk9vmb)
- [Law500K.dat](https://sfsu.box.com/s/1htn01fmbyvang59u8mxu5igvqdutnbw)
- [Law_Full.dat](https://sfsu.box.com/s/9547tfcl6qadznmwbhpeqh4ky1mrnkh0)

threads:  is how many threads you will dispatch to process the data.  Remember that you have to submit common runs with 1, 2, 4, and 8 threads, using the Law500K.dat file.  Common runs means with the same sub-field and sub-field values between each thread run.

subfield: is either `analysis_neighborhood`  or  `police_district`

The subfield value's (1 and 2) is one of the field values from `analysis_neighborhood`  or  `police_district`

|                             |       Police Districts        |                        |
| --------------------------- | ----------------------------- | ---------------------- |
| BAYVIEW                     | NORTHERN                      | SOUTHERN               |
| CENTRAL                     | PARK                          | TARAVAL                |
| INGLESIDE                   | RICHMOND                      | TENDERLOIN             |
| MISSION                     |                               |                        |


|                             |     Neighborhoods             |                        |
| --------------------------- | ----------------------------- | ---------------------- |
| Bayview Hunters Point       | Lincoln Park                  | Potrero Hill           |
| Bernal Heights              | Lone Mountain/USF             | Presidio               |
| Castro/Upper Market         | Marina                        | Presidio Heights       |
| Chinatown                   | McLaren Park                  | Russian Hill           |
| Excelsior                   | Mission                       | Seacliff               |
| Financial District/South Beach | Mission Bay                 | South of Market        |
| Glen Park                   | Nob Hill                      | Sunset/Parkside        |
| Golden Gate Park            | Noe Valley                    | Tenderloin             |
| Haight Ashbury              | North Beach                   | Treasure Island        |
| Hayes Valley                | Oceanview/Merced/Ingleside    | Twin Peaks             |
| Inner Richmond              | Outer Mission                 | Visitacion Valley      |
| Inner Sunset                | Outer Richmond                | West of Twin Peaks     |
| Japantown                   | Pacific Heights               | Western Addition       |
| Lakeshore                   | Portola                       |                        |



**Hints** Do not forget to protect critical sections. Make sure you use thread safe library calls. Do not use pipes. You must use mutex locks.

Do a writeup using the template in PDF format that includes your approach in detail, issues, resolutions and an **analysis** of the results. The **analysis** is in two parts.  First part is to analyze the output and explain if the results of the statistics you generated were what you expect and explain them in a brief paragraph (i.e. what do the statictics tell us about police reponse time).  Part two of the analysis is to Explain and reflect on why the times for the different runs are what they are, how does each run compare with the others. Also include the compilation and execution output from your program in the writeup. Your execution output must include at least 4 common runs of, 1 thread, 2 threads, 4 threads, and 8 threads, using the Law500K.dat file. Common runs means with the same sub-field and sub-field values between each thread run.  Submit all your source code file(s), Makefile and PDF writeup into GitHub. The writeup (PDF) should also be submitted to Canvas.

**Note** Do not change any of the settings of the virtual machine.

## **Extra Credit**: 
Redisplay the 9 pages but show the data using a Box and Wisker plot on the terminal.
- The | on the left is the Lower Bound, the | on the right is the Upper Bound
- The [ is the Q1 value the ] is the Q3 value
- The | in the middle is the Median
- The x is the mean
- The = just fills in the rest of the "box"
- The o's are outliers

```
oo  |-------[===|==x=]--------------| o
|     |     |     |     |     |     |     |
0     5     10    15    20    25    30    35
```

## **Grading Rubric**
| Grade Item                     | Grade Points |
|--------------------------------|--------------|
| Header(s)                      |            2 |
| Documented Code                |            8 |
| Command Line Arg.              |            5 |
| Processing of the header file  |            5 |
| Used Threads                   |           15 |
| Used and Displayed Timer Code  |            5 |
| Runs to Completion             |           10 |
| Correct Output                 |           10 |
| Double Thread 1/2 time         |           15 |
| Proper Use of Critical Section |           10 |
| Writeup                        |           15 |
| Extra Credit for Box Plot      |            5 |



## Information about the data we are using
The is from DataSF entitles Law Enforcement Dispatched Calls for Service: Closed.  The URL to obtain this data is [https://data.sfgov.org/Public-Safety/Law-Enforcement-Dispatched-Calls-for-Service-Close/2zdj-bwza/about_data](https://data.sfgov.org/Public-Safety/Law-Enforcement-Dispatched-Calls-for-Service-Close/2zdj-bwza/about_data) (As of 2/29/24)

| Column Name	| Description                                                |	Type    |
|-------------|------------------------------------------------------------|----------|
| cad_number	| Unique 9-digit number assigned by the 911 Dispatch Center (DEM) to this call. Both Police and Fire Depts use these numbers. | Plain Text |
| dup_cad_number | If a call is later determined to be related to a single incident that received multiple dispatched calls, this field will be populated with the related CAD number. | Plain Text |
| pd_incident_report | If a police incident report is created as a result of this call the report number will be found here. | Plain Text |
| received_datetime | Date and time when the 911 Dispatch Center receives the call or, when onview_flag = Y, an officer initiates an on-view incident | Date & Time |
| entry_datetime | Date and time when the 911 operator submits the initial call information into the CAD system or, when onview_flag = Y, an officer initiates an on-view incident | Date & Time |
| dispatch_datetime | Date and time when the 911 operator dispatches the first unit to the location of the incident or, when onview_flag = Y, an officer initiates an on-view incident | Date & Time |
| enroute_datetime | Date and time when the first unit acknowledges the dispatch and records that they are enroute to the location of the incident or, when onview_flag = Y, an officer initiates an on-view incident | Date & Time |
| onscene_datetime | Date and time the first unit records arriving at the location of the incident or, when onview_flag = Y, an officer initiates an on-view incident | Date & Time |
| close_datetime | Date and time when the last unit responding closes the call | Date & Time |
| call_type_original | Call type code as originally coded/selected by dispatcher | Plain Text |
| call_type_original_desc | Standard description of original call type | Plain Text |
| call_type_original_notes | Any notes regarding the original call type entered by the dispatcher | Plain Text |
| call_type_final | Final call type code as coded/selected by dispatcher. For closed calls this is the last call type code. For open calls this code may continue to change. | Plain Text |
| call_type_final_desc | Standard description of final call type | Plain Text |
| call_type_final_notes | Any notes regarding the final call type entered by the dispatcher | Plain Text |
| priority_original | Priority level originally coded by dispatcher | Plain Text |
| priority_final | Final priority level for closed calls and most recent priority level for open calls | Plain Text |
| agency | Primary agency assigned (Police, Sheriff, or MTA). Multiple units from different agencies may respond, but only one agency will be identified as the primary responder. | Plain Text |
| disposition | Disposition of call | Plain Text |
| onview_flag | Indicates whether the associated cad_number originated from a 911 call (onview_flag = N) or from a law enforcement unit viewing an incident in the field (onview_flag = Y). | Plain Text |
| sensitive_call | Indicates whether the call is of a sensitive nature. Location information & call notes are not provided for sensitive calls. | Checkbox |
| intersection_name | Name of intersection used to anonymize the incident location | Plain Text |
| intersection_id | Unique ID for the intersection_point used to anonymize the incident location | Plain Text |
| intersection_point | Geographic point representing the anonymized incident location (nearest intersection) | Point |
| supervisor_district | Supervisor District based on anonymized intersection coordinates | Plain Text |
| analysis_neighborhood | Analysis Neighborhoods based on anonymized intersection coordinates | Plain Text |
| police_district | Police District based on anonymized intersection coordinates | Plain Text |
| data_as_of | Timestamp when data for this call was last extracted from the source CAD system | Date & Time |
| data_updated_at | Timestamp when data for this call was last processed | Date & Time |
| data_loaded_at | Timestamp when data for this call was last loaded here (into the open data portal) | Date & Time |
| source_filename | Name of the file from which this call was extracted | Plain Text |
	
