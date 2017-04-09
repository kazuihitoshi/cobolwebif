       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           s.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       DATA                  DIVISION.
       FILE                  SECTION.
      *
       WORKING-STORAGE SECTION.
       copy flag replacing flag- by TR-.
      *------------------------------------------------
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
      *
          display 's.cob'.
      *    move 1 to flag-p of flag-rec.            
      *    move 0 to flag-p of flag-rec.
           set TR-1 to true.
      *
      *
          STOP RUN.
   




