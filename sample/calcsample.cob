       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           calcsample.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       DATA                  DIVISION.
       WORKING-STORAGE SECTION. 
       01 webif-rec.
         02 filler pic x(20) value 'WMODE'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 wmode  pic x value space.
         02 filler pic x value low-value.
         02 filler pic x(20) value 'DATA1'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 data1  pic x(12) value space.
         02 filler pic x value low-value.
         02 filler pic x(20) value 'DATA2'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 data2 pic x(12) value space.
         02 filler pic x value low-value.
         02 filler pic x(20) value 'RESULT'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 result pic x(13) value space.
         02 filler pic x value low-value.
         02 filler pic x value low-value.
       01  webif-in.
         02 wmode  pic x.
         02 data1  pic 9(12).
         02 data2  pic s9(12).
         02 result pic s9(12).
       01  webif-ot.
         02 wmode  pic x.
         02 data1  pic zzzzzzzzzzz9.
         02 data2  pic zzzzzzzzzzz9.
         02 result pic -zzzzzzzzzzz9.
       77 w-x pic x.
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
          call 'WEB_GET_QUERY_STRING'.
          call 'WEB_POP' using webif-rec.
          move corr webif-rec to webif-in.
          evaluate wmode of webif-in
           when 'a'
            compute result of webif-in = data1 of webif-in +
                                       data2 of webif-in
           when 's'
            compute result of webif-in = data1 of webif-in -
                                       data2 of webif-in
           when 'm'
            compute result of webif-in = data1 of webif-in *
                                       data2 of webif-in
           when 'd'
            if data2 of webif-in = zero then
              move zero to result of webif-in
             else
            compute result of webif-in = data1 of webif-in /
                                       data2 of webif-in
            end-if
          end-evaluate.
          move corr webif-in to webif-ot.
          move corr webif-ot to webif-rec.
          call 'WEB_PUSH' using webif-rec.
          call 'WEB_SHOW'.
          STOP RUN.






