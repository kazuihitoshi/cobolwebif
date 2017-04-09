       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           calcsample.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       DATA                  DIVISION.
       WORKING-STORAGE SECTION. 
       01 webif-rec.
         02 filler pic x(20) value 'DATA'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 wdata  pic x(20) value space.
         02 filler pic x value low-value.
         02 filler pic x(20) value 'PROGRAMNAME'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 wprogramname  pic x(20) value space.
         02 filler pic x value low-value.
         02 filler pic x value low-value.
         02 filler pic x value low-value.
       77 w-x pic x.
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
          call 'WEB_GET_QUERY_STRING'.
          call 'WEB_POP' using webif-rec.
          move 'progsample.exe' to wdata of webif-rec.
          move 'progsample2.exe' to wprogramname of webif-rec.

          call 'WEB_PUSH' using webif-rec.
          call 'WEB_SHOW'.
          STOP RUN.






