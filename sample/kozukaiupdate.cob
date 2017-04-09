       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           kozukaiupdate.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       FILE-CONTROL.
        select kozukai-file assign to "kozukai.dat"
           organization is indexed access mode is sequential
           record key is kozukai-key 
           file status is kozukai-file-fs.
       DATA                  DIVISION.
       FILE                  SECTION.
       fd kozukai-file.
       copy kozukaifile.
    
       WORKING-STORAGE SECTION. 

       copy kozukaiwebif.

       copy web-lock.

       77   w-x pic x.

       77 rc     pic s9(10) binary.

       01 kozukai-file-fs  pic 9(2).

       01 i  pic 9(10).
       01 kozukai-eof      pic x(3) value space.
       01 logwrite-rec.
        02 logwrite-dat  pic x(80).
        02 filler  pic x value low-value.

       PROCEDURE             DIVISION.
       MAIN-PROC             SECTION.
       MAIN-PROC-1.

          call 'WEB_GET_QUERY_STRING'.
          call 'WEB_POP' using kozukai-web-if.
          if wscreenname of kozukai-web-if = space then
            move 'kozukai.html' to wscreenname of kozukai-web-if
          end-if.

          move corr kozukai-web-if to kozukai-web-in.

      *    display 'delete1:' delete1 of kozukai-web-if ':'.
      *    display 'delete1:' delete1 of kozukai-web-in ':'.

          initialize kozukai-rec.

          move 'kozukai_lock'   to lc-lock-file.
          set  lc-lock-exclusive to true.
          call 'WEB_LOCK' using lc-web-lock rc.
          if wmode of kozukai-web-in = space then
            move '参照' to wmode of kozukai-web-in
          end-if.
          evaluate wmode of kozukai-web-in
           when '参照'
             perform select-proc

           when '追加'
             move wdate-rec of kozukai-web-in
                   to wdate-rec of kozukai-rec
             move wdate-sec     of kozukai-web-in
                   to wdate-sec of kozukai-rec
             move wcategory of kozukai-web-in
                   to wcategory of kozukai-rec
             move in-or-out of kozukai-web-in
                   to in-or-out of kozukai-rec
             move money     of kozukai-web-in
                   to money of kozukai-rec
             move note     of kozukai-web-in
                   to note of kozukai-rec
             open extend kozukai-file
             if kozukai-file-fs not = zero then
               open output kozukai-file
             end-if
             write kozukai-rec
             close kozukai-file
             perform select-proc
           when 'UPDATE'
             continue
           when '更新選択'
             open input kozukai-file
             move wdate-rec of kozukai-web-in
                   to kozukai-key of kozukai-rec(1:8)
             move wdate-sec of kozukai-web-in
                   to wdate-sec   of kozukai-rec

             start kozukai-file
                key is >= kozukai-key of kozukai-rec

             read kozukai-file

             move wdate-rec of kozukai-rec(1:8) to 
                               wdate-rec of kozukai-web-in
             move wdate-sec of kozukai-rec to 
                               wdate-sec of kozukai-web-in
             move wcategory of kozukai-rec to 
                               wcategory of kozukai-web-in
             move in-or-out of kozukai-rec to
                               in-or-out of kozukai-web-in
             move money     of kozukai-rec to
                               money     of kozukai-web-in
             move note      of kozukai-rec to
                               note      of kozukai-web-in

             move corr kozukai-web-in to kozukai-web-ot
             move corr kozukai-web-ot to kozukai-web-if
             close kozukai-file
           when '削除'
      *       display '------------------------------------'
      *       display ':' delete1 of kozukai-web-if ':'
             open i-o kozukai-file
             perform varying i from 1 by +1 until i > 10 
               if  delete-array (i) not = space then
                 move delete-array (i) to
                     kozukai-key of kozukai-rec

                 move delete-array(i) to logwrite-dat
                 call 'WEB_LOGWRITE' using logwrite-rec

                 start kozukai-file
                   key is >= kozukai-key of kozukai-rec

                 read kozukai-file

                 if kozukai-file-fs = zero then
                   if kozukai-key = delete-array(i) then
                     delete kozukai-file
                   end-if
                 end-if
                 move space to delete-array(i)
      *          display 'kozukai-file-fs:' kozukai-file-fs ':'
               end-if
             end-perform
             close kozukai-file
             perform select-proc
          end-evaluate.

          call 'WEB_PUSH' using kozukai-web-if.
          call 'WEB_SHOW'.

          call 'WEB_UNLOCK' using lc-web-lock rc.
          STOP RUN.

       select-proc section.
             move sel-date-rec of kozukai-web-in 
                            to wdate-rec of kozukai-rec
             move sel-date-sec of kozukai-web-in
                            to wdate-sec of kozukai-rec
             open input kozukai-file
             move sel-date-rec of kozukai-web-in 
                         to wdate-rec of kozukai-rec
             move sel-date-sec of kozukai-web-in
                         to wdate-sec of kozukai-rec
             start kozukai-file
                key is >= kozukai-key of kozukai-rec
             end-start
             initialize table-rec
             move space to kozukai-eof
             read kozukai-file
               at end move 'EOF' to kozukai-eof
             end-read
             perform varying i from 1 by +1  until i > 10 
                                 or kozukai-eof = 'EOF'
               move kozukai-rec to table-array (i)
               read kozukai-file
                 at end
                  move 'EOF' to kozukai-eof
               end-read
                
             end-perform
             close kozukai-file

             move corr kozukai-web-in to kozukai-web-ot

             move corr kozukai-web-ot to kozukai-web-if .

             exit.






