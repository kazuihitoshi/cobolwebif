       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           cookie.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       DATA                  DIVISION.
       FILE                  SECTION.
      *
       WORKING-STORAGE SECTION.
       01 http-cookie-rec.
        02 filler  pic x(10) value 'DATA1'.
        02 filler  pic x value low-value.
        02 filler pic x value space.
        02 filler pic x value low-value.
        02 data1  pic x(16) value space.
        02 filler pic x value low-value.
        02 filler pic x value low-value.

       01 web-if-rec.
        02 filler pic x(15) value 'SCREENNAME'.
        02 filler pic x value low-value.
        02 filler pic x value space.
        02 filler pic x value low-value.
        02 wscreenname pic x(80) value 'cookie.html'.
        02 filler pic x value low-value.
        02 filler pic x(10) value 'DATA1'.
        02 filler pic x value low-value.
        02 filler pic x value space.
        02 filler pic x value low-value.
        02 data1  pic x(16) value space.
        02 filler pic x value low-value.
        02 filler pic x value low-value.

       COPY web-cookie.
       01 web-rc   pic 9(10) binary.
      *------------------------------------------------
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
      *
          call 'WEB_GET_QUERY_STRING'.
          call 'WEB_POP'        using web-if-rec.
          move 'cookie.html' to wscreenname of web-if-rec.
          if data1 of web-if-rec  = SPACE then
           call 'WEB_POP_COOKIE' using http-cookie-rec
           move data1 of http-cookie-rec to data1 of web-if-rec
          else
           initialize WEB-COOKIE
           move 'DATA1'   to WEB-NAME
           move  data1 of web-if-rec to WEB-VALUE
           call  'WEB_SET_COOKIE' using WEB-COOKIE WEB-RC

           initialize WEB-COOKIE
           move 'DATA2'   to WEB-NAME
           move  'test save' to WEB-VALUE
           call  'WEB_SET_COOKIE' using WEB-COOKIE WEB-RC
          end-if.
          call 'WEB_PUSH' using web-if-rec.


          call 'WEB_SHOW'

      *
          STOP RUN.


