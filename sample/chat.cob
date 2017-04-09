       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           chat.
       AUTHOR.               ys62.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       FILE-CONTROL.
        select chat-file assign to "./chat.dat"
          organization line sequential access sequential
          file status is chat-file-fs.
       DATA                  DIVISION.
       FILE                  SECTION.
       fd chat-file.
       01 chat-rec.
          02 wcolor   pic x(30).
          02 wname    pic x(30).
          02 wtime    pic x(20).
	  02 wdata    pic x(200).
      *
       WORKING-STORAGE SECTION.
       77 chat-file-fs     pic 9(02).

       01 wchat-rec.
          02 wchat-rec-tab occurs 40.
           03 wcolor   pic x(30).
           03 wname    pic x(30).
	   03 wdata    pic x(200).
           03 wtime    pic x(20).
         
       copy chat-param.

       copy web-cookie.

       01 http-cookie.
         02 filler pic x(10) value 'NAME'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 wname  pic x(30) value space.
         02 filler pic x value low-value.
         02 filler pic x(10) value 'COLOR'.
         02 filler pic x value low-value.
         02 filler pic x value space.
         02 filler pic x value low-value.
         02 wcolor pic x(30) value space.
         02 filler pic x value low-value.
         02 filler pic x value low-value.
      
       77 ret-value       pic s9(9).
      
       77 w-x             pic x(20).
      
       77 i               pic 9(2).
      *
       77 wwdate           pic x(6).
       77 wwtime           pic x(8).
       77 chat-rec-eof     pic x(3) value space.
       01 arg-command-line.
        02 arg-command-line-d pic x(160) value space.
        02 filler             pic x      value zero.
       77 rc pic s9(10) usage binary.
      *----------------------------------------------------------------
       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.
      *
      *   accept arg-command-line-d from command-line.
          call 'WEB_GET_QUERY_STRING'.
          call 'WEB_POP' using chat-param-if.
          call 'WEB_POP_COOKIE' using http-cookie.
          move 1 to i.

          accept wwdate from date.
          accept wwtime from time.
      *   ハンドル名がなければログインメッセージを出して終了
          if wname of chat-param-if = SPACE then
             move 'chatlogin.html' to wscreenname of chat-param-if
             move 'up'             to wtarget     of chat-param-if
             move wcolor of chat-rec to wcolor of wchat-rec-tab (i)
             move wname  of chat-rec to wname  of wchat-rec-tab (i)
             move wname  of http-cookie to wname  of chat-param-if
             move wcolor of http-cookie to wcolor of chat-param-if
             perform SET-COOKIE
             call 'WEB_PUSH' using chat-param-if
             call 'WEB_SHOW'
             go to 99
          end-if.
          if wname of chat-param-if not = SPACE and
             wscreenname of chat-param-if = 'chatlogin.html' then
             move 'chatsend.html'   to wscreenname of chat-param-if
             move 'down'        to wtarget     of chat-param-if
             PERFORM SET-COOKIE
             call 'WEB_PUSH' using chat-param-if
             call 'WEB_SHOW'
             go to 99
          end-if.
      *
          PERFORM SET-COOKIE.

      *   move 'chatsend.html'   to wscreenname of chat-param-if.
      *   move 'down'        to wtarget     of chat-param-if.
          move 'chatlog.html' to wscreenname of chat-param-if.
          call 'WEB_PUSH' using chat-param-if.
          perform SHOW-LOG.
          call 'WEB_SHOW'.
      *    go to 99.          
      
       99.
          stop run.
      *
       SET-COOKIE SECTION.
       00.
          initialize  web-cookie.
          move 'NAME'                 to web-name.
          move wname of chat-param-if to web-value.
          move 'Fri, 31-Dec-2030 23:59:59' to web-expires.
          call 'WEB_SET_COOKIE' using WEB-COOKIE RC.

          initialize  web-cookie.
          move 'COLOR'                 to web-name.
          move wcolor of chat-param-if to web-value.
          move 'Fri, 31-Dec-2030 23:59:59' to web-expires.
          call 'WEB_SET_COOKIE' using WEB-COOKIE RC.
       99.
          EXIT.

       SHOW-LOG section.
          move corr chat-param-if to chat-param-in.
          initialize wchat-rec.
          move space to chat-rec-eof.
          open  input chat-file.
          if chat-file-fs = 0 then
            read chat-file 
            at end
              move 'EOF' to chat-rec-eof
            end-read
            move zero to i
            perform until chat-rec-eof = 'EOF'
              add 1 to i
              move wcolor of chat-rec to wcolor of wchat-rec-tab (i)
              move wname  of chat-rec to wname  of wchat-rec-tab (i)
              move wdata  of chat-rec to wdata  of wchat-rec-tab (i)
              move wtime  of chat-rec to wtime  of wchat-rec-tab (i)
              read chat-file
               at end 
                 move 'EOF' to chat-rec-eof
              end-read
            end-perform
            move wchat-rec to dat-table of chat-param-in-tab
          end-if.
          close chat-file.
      
          if wsenddata of chat-param-in not = space 
            and wmode  of chat-param-in not = "Reload" then
           perform varying i from 40 by -1 until i <= 1
        
            move wname of dat-table-item of chat-param-in-tab ( i - 1 ) 
              to wname of dat-table-item of chat-param-in-tab (i)
            move wdata of dat-table-item of chat-param-in-tab ( i - 1 ) 
              to wdata of dat-table-item of chat-param-in-tab (i)
            move wtime of dat-table-item of chat-param-in-tab ( i - 1 ) 
              to wtime of dat-table-item of chat-param-in-tab (i)
            move wcolor of dat-table-item of chat-param-in-tab( i - 1 ) 
              to wcolor of dat-table-item of chat-param-in-tab (i)
           end-perform
           move wsenddata of chat-param-in-tab to
           wdata of dat-table-item of dat-table of chat-param-in-tab(1)
           move wname     of chat-param-in to 
            wname of dat-table-item of chat-param-in-tab (1)
           move wcolor   of chat-param-in to 
            wcolor of dat-table-item of chat-param-in-tab (1)
           string 
              wwdate(1:2) delimited size 
              '/'         delimited size
              wwdate(3:2) delimited size 
              '/'         delimited size
              wwdate(5:2) delimited size 
              ' '         delimited size
              wwtime(1:2) delimited size 
              ':'         delimited size
              wwtime(3:2) delimited size 
              ':'         delimited size
              wwtime(5:2) delimited size 
            into wtime of dat-table-item of chat-param-in-tab (1)
           end-string
           open output chat-file
           perform varying i from 1 by 1 until i > 40
             move wcolor of dat-table of chat-param-in-tab (i) 
               to wcolor of chat-rec
             move wname  of dat-table of chat-param-in-tab (i) 
               to wname  of chat-rec
             move wcolor of dat-table of chat-param-in-tab (i) 
               to wcolor of chat-rec
             move wdata  of dat-table of chat-param-in-tab (i) 
               to wdata  of chat-rec
             move wtime  of dat-table of chat-param-in-tab (i) 
               to wtime  of chat-rec
             write chat-rec
           end-perform
           close chat-file
           move space to wsenddata of chat-param-in
          end-if.
      
          move corr chat-param-in  to chat-param-ot.
          move corr chat-param-ot  to chat-param-if.
      
          call 'WEB_PUSH' using chat-param-if.
      *   call 'WEB_SHOW'.
       99.
          exit.
