       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           multiform.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       file-control.
       DATA                  DIVISION.
      *
       WORKING-STORAGE SECTION.
       01 webif-rec.
         02 filler  pic x(20) value 'DATA1'.
         02 filler  pic x value low-value.
         02 filler  pic x value space.
         02 filler  pic x value low-value.
         02 wdata1 pic x(15) value space.
         02 filler  pic x value low-value.
         02 filler  pic x(20) value 'DATA2'.
         02 filler  pic x value low-value.
         02 filler  pic x value space.
         02 filler  pic x value low-value.
         02 wdata2 pic x(15) value space.
         02 filler  pic x value low-value.
         02 filler  pic x value low-value.

      *------------------------------------------------
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
      *

          call 'WEB_GET_QUERY_STRING'.

          call 'WEB_POP' using webif-rec.
          
          call 'WEB_PUSH' using webif-rec.

          call 'WEB_SHOW' using webif-rec.

          STOP RUN.

