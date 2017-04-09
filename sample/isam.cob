       IDENTIFICATION        DIVISION.
       PROGRAM-ID.           isam.
       AUTHOR.               kazui.
       ENVIRONMENT           DIVISION.
       CONFIGURATION         SECTION.
       INPUT-OUTPUT          SECTION.
       FILE-CONTROL.
        select test-file assign to "test.dat"
          organization is indexed access mode is sequential
          record key is test-key 
          file status is test-file-fs.
       DATA                  DIVISION.
       FILE                  SECTION.
       fd test-file.
       01 test-rec.
         02 test-key  pic x(8).
         02 test-dat  pic x(20).
         02 test-dat2 pic x(20).
       WORKING-STORAGE SECTION. 
       01 test-file-fs  pic 9(2).
       01 wcommand-line pic x(80).
       procedure division.
       main-proc section.
       00.
          move 'read' to wcommand-line.
          if wcommand-line = 'write' then
            open output test-file
            initialize test-rec
            move '00000001' to test-key 
            move all 'a'    to test-dat
            move all 'A'    to test-dat2
            write test-rec
            move '00000002' to test-key 
            move all 'b'    to test-dat
            move all 'B'    to test-dat2
            write test-rec
            move '00000003' to test-key 
            move all 'c'    to test-dat
            move all 'C'    to test-dat2
            write test-rec
            close test-file
          else
            open input test-file
            move '00000002' to test-key
            start test-file
              key is >= test-key
            end-start
            display 'start access: test-file-fs:' test-file-fs
            read test-file
            display 'test-file-fs:' test-file-fs ':' test-rec ':'
            move '00000001' to test-key
            start test-file
              key is = test-key
            end-start
            read test-file
            display 'read test-file-fs:' test-file-fs ':' test-rec ':'
            close test-file
          end-if
          .         

       99.
          stop run.

