       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           addsample.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       file-control.
        select counter-file assign to "counter.dat"
           file status is counter-file-fs.
       DATA                  DIVISION.
       FILE                  SECTION.
       fd counter-file.
       01 counter-rec.
         02 counter-dat    pic  9(12).
      *
       WORKING-STORAGE SECTION.
       01 webif-rec.
         02 filler  pic x(20) value 'COUNTER'.
         02 filler  pic x value low-value.
         02 filler  pic x value space.
         02 filler  pic x value low-value.
         02 wcounter pic x(15) value space.
         02 filler  pic x value low-value.
         02 filler  pic x value low-value.

       77 counter-file-fs pic 9(2).
       COPY web-lock.


       77 counter-format  pic zzz,zzz,zzz,zz9.

       77 rc     pic s9(10) binary.
      *------------------------------------------------
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
      *
          move 'counter_lock'   to lc-lock-file.
          set  lc-lock-exclusive to true.
          call 'WEB_LOCK' using lc-web-lock rc.

          call 'WEB_GET_QUERY_STRING'.

          call 'WEB_POP' using webif-rec.
          
          open input counter-file.
          if counter-file-fs not = zero then
             move zero to counter-dat
          else
             read counter-file
             close counter-file             
          end-if.
          add 1 to counter-dat.
          move counter-dat to counter-format.

          move counter-format to wcounter of webif-rec.

          open output counter-file.

          write counter-rec.

          close counter-file.

          call 'WEB_PUSH' using webif-rec.

          call 'WEB_SHOW' using webif-rec.


          call 'WEB_UNLOCK' using lc-web-lock rc.
      
          STOP RUN.

