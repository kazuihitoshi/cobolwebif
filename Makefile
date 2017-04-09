WEBLIB :=  libweb.a
OBJECTDIR:= object
#COBLIBS := -L/usr/local/lib -lcob -lm -ldb1 -lgmp -lltdl -lncurses -lc
COBLIBS := -lcob -lm -ldb -lgmp -lltdl -lc
COBOPT	:= -I /users/hitoshi/YSK/std/cobol -I ./
WEBLIBS	:= -L./ -lweb -lc
LOCALVER:=web.0.3.10.3

OBJECTS := ini.o sstr.o log.o gurdio.o pipe_exe.o web.o

CFLAGS  = -DDEFAULT_LOGFILENAME=\"scg.log\" -I src
#CFLAGS  = -DNOLOG -DDEFAULT_LOGFILENAME=\"/tmp/scg.log\" -I src
#CFLAGS  = -DDEFAULT_LOGFILENAME=\"scg.log\" -I src
#CFLAGS  = -DDEFAULT_LOGFILENAME=\"/tmp/scg.log\" -I src

all: $(WEBLIB) addsample.cgi calcsample.cgi cobolweb.dvi cookie.cgi

cobolweb.dvi: doc/cobolweb.tex doc/history.tex doc/gaiyo.tex doc/webapi.tex doc/web_get_query_string.tex doc/web_put_query_string.tex doc/web_variable.tex doc/web_pop.tex doc/web_push.tex doc/web_show.tex doc/web_lock.tex doc/special_variable.tex doc/special_programname.tex doc/special_screenname.tex doc/html_text.tex doc/html_submit.tex doc/html_select_option.tex doc/html_textarea.tex doc/cgicobolfurumai.tex doc/addsample.tex doc/calcsample.tex doc/longtimerequest.tex doc/platwindows.tex doc/platlinux.tex sample/calcsample_html.tex sample/addsample_html.tex doc/chat.tex doc/web_cookie.tex doc/cookie.tex
	(p=`pwd`;cd `dirname $<`;\
	platex `basename $<`;	platex `basename $<`;\
	cp -p  `basename $< .tex`.dvi $$p)
cobolweb.ps:cobolweb.dvi
	dvips $<
cobolweb.pdf:cobolweb.dvi
	dvipdfm -p a4 -o $@ $<

doc/addsample.tex:sample/addsample.cob sample/addsample.html sample/addsample_html.tex
	echo "{\gt html} addsample.html" > $@
	echo "" >>$@
	echo "{\footnotesize " >> $@
	./bin/txttotex.sh -tabular sample/addsample.html >> $@
	echo "}" >> $@
	echo "" >>$@
	echo "\vspace{1em}" >>$@
	echo "{\gt cobol source} addsample.cob">>$@
	echo "" >>$@
	echo "{\footnotesize" >> $@
	./bin/txttotex.sh -tabular sample/addsample.cob  >> $@
	echo "}" >> $@
	echo "" >> $@
	cat sample/addsample_html.tex >> $@

doc/calcsample.tex:sample/calcsample.cob sample/calcsample.html sample/calcsample_html.tex
	echo "{\gt html} calcsample.html" > $@
	echo "" >>$@
	echo "{\footnotesize " >> $@
	./bin/txttotex.sh -tabular sample/calcsample.html >> $@
	echo "}" >> $@
	echo "" >>$@
	echo "\vspace{1em}" >>$@
	echo "{\gt cobol source} calcsample.cob">>$@
	echo "" >>$@
	echo "{\footnotesize" >> $@
	./bin/txttotex.sh -tabular sample/calcsample.cob  >> $@
	echo "}" >> $@
	echo "" >> $@
	cat sample/calcsample_html.tex >> $@

doc/cookie.tex:sample/cookie.cob sample/cookie.html sample/cookie_html.tex
	echo "{\gt html} cookie.html" > $@
	echo "" >>$@
	echo "{\footnotesize " >> $@
	./bin/txttotex.sh -tabular sample/cookie.html >> $@
	echo "}" >> $@
	echo "" >>$@
	echo "\vspace{1em}" >>$@
	echo "{\gt cobol source} cookie.cob">>$@
	echo "" >>$@
	echo "{\footnotesize" >> $@
	./bin/txttotex.sh -tabular sample/cookie.cob  >> $@
	echo "}" >> $@
	echo "" >> $@
	cat sample/cookie_html.tex >> $@

doc/chat.tex:sample/chat.cob sample/chat-param sample/chat.html sample/chatlogin.html sample/chatlog.html sample/chatsend.html 
	echo "{\gt html} chat.html" > $@
	echo "" >>$@
	echo "{\footnotesize " >> $@
	echo "\begin{verbatim}" >> $@
	cat  sample/chat.html >> $@
	echo "\end{verbatim}" >> $@
	echo "" >>$@
	echo "\vspace{1em}" >>$@
	echo "{\gt cobol source} chat.cob">>$@
	echo "" >>$@
	echo "{\footnotesize" >> $@
	echo "\begin{verbatim}" >>$@
	expand sample/chat.cob  >> $@
	echo "\end{verbatim}" >> $@
	echo "{\gt copy file} chat-param">>$@
	echo "" >>$@
	echo "{\footnotesize" >> $@
	echo "\begin{verbatim}">>$@
	expand sample/chat-param  >> $@
	echo "\end{verbatim}" >> $@
	echo "" >> $@

xdvi:	cobolweb.dvi
	xdvi-ja $<

$(WEBLIB): $(OBJECTS)
	ar rvf $@ $(OBJECTS)

test:$(PROGRAM)
	export QUERY_STRING="SCREENNAME=test.html&PROGRAM=test.prg&SERIAL_NO1=1111&SERIAL_NO3=";./scg  

sample: addsample.cgi textsample.cgi calcsample.cgi chat.cgi counter.cgi
	touch sample
winsample: addsample.exe textsample.exe calcsample.exe chat.exe counter.cgi
	touch winsample

install: libweb.a copy/web-cookie copy/web-cookie.cob copy/web-lock copy/web-lock.cob
	mkdir -p /usr/local/lib
	mkdir -p /usr/local/copy
	mkdir -p /usr/local/bin
	install -m 644 -o root -g root libweb.a            /usr/local/lib
	install -m 644 -o root -g root copy/web-cookie     /usr/local/copy
	install -m 644 -o root -g root copy/web-cookie.cob /usr/local/copy
	install -m 644 -o root -g root copy/web-lock       /usr/local/copy
	install -m 644 -o root -g root copy/web-lock.cob   /usr/local/copy
	install -m 755 -o root -g root tools/htmltocopy.sh /usr/local/bin
	touch install

sampleinstall: addsample.cgi calcsample.cgi sample/addsample.html sample/calcsample.html
	cp -p addsample.cgi  ~cobol/public_html/cgi-bin
	cp -p calcsample.cgi ~cobol/public_html/cgi-bin
	cp -p sample/addsample.html ~cobol/public_html/cgi-bin
	cp -p sample/calcsample.html ~cobol/public_html/cgi-bin

liblinux: libweb.a
	/bin/rm -r -f liblinux
	mkdir -p                liblinux/usr/local/lib
	mkdir -p                liblinux/usr/local/include
	cp -p libweb.a          liblinux/usr/local/lib
	cp -p copy/web-lock     liblinux/usr/local/include
	cp -p copy/web-lock.cob liblinux/usr/local/include
	cd liblinux;tar zcvf ../linux$(LOCALVER).tar.gz .
	touch liblinux

libwin: libweb.a
	/bin/rm -r -f libwin
	mkdir -p                libwin/usr/local/lib
	mkdir -p                libwin/usr/local/include
	cp -p libweb.a          libwin/usr/local/lib
	cp -p copy/web-lock     libwin/usr/local/include
	cp -p copy/web-lock.cob libwin/usr/local/include
	cd libwin;tar zcvf ../win$(LOCALVER).tar.gz .
	touch libwin
clean:
	rm -f *.o $(PROGRAM) *.bak calcsample.cgi addsample.cgi libweb.a *.dvi *.aux *.log *.lzh *.pdf *.ps *.toc
	find . -name '*~' -exec /bin/rm {} \;
	rm -r -f libwin liblinux
backup:
	tar zcvf web.tar.gz Makefile bin doc src sample sample2
arc:
	mkdir -p $(LOCALVER)
	tar cf - Makefile copy/* bin/* doc/*.tex src/*.c src/*.h sample/*.cob sample/chat-param sample/*.html sample/*.tex tools/* |(cd $(LOCALVER);tar -xvf - )
	tar zcvf $(LOCALVER).tar.gz $(LOCALVER)
%.cgi:%.o
	export COB_LDADD="$(WEBLIBS)"; cobc -o $@ $<
%.o:sample/%.cob
	cobc  -o $@ -c $< -static -fmain -Isample -Icopy
%.cgi:sample/%.cob
	export COB_LDADD="$(WEBLIBS)";cobc  -o $@ $< -static -fmain -Icopy -Isample
%.exe:sample/%.cob
	export COB_LDADD="$(WEBLIBS)";cobc  -o $@ $< -static -fmain -Icopy -Isample
%.o:src/%.c
	gcc $(CFLAGS) -c $< 

