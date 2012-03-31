%%
%% This is file `liuthesis.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% liuthesis.dtx  (with options: `class')
%% 
%% IMPORTANT NOTICE:
%% 
%% For the copyright see the source file.
%% 
%% Any modified versions of this file must be renamed
%% with new filenames distinct from liuthesis.cls.
%% 
%% For distribution of the original source see the terms
%% for copying and modification in the file liuthesis.dtx.
%% 
%% This generated file may be distributed as long as the
%% original source files, as listed above, are part of the
%% same distribution. (The sources need not necessarily be
%% in the same archive or directory.)
\ProvidesClass{liuthesis}[2007/01/13 liuthesis Class v. 1.3]
\NeedsTeXFormat{LaTeX2e}
\RequirePackage{lmodern}
\RequirePackage[T1]{fontenc}
\RequirePackage[latin1]{inputenc}
\RequirePackage[swedish, english]{babel}
\newcommand*{\liuthesisDefinition}[2]{%
  \ClassInfo{liuthesis}{Loaded definition file '\CurrentOption.ltd'}%
  \newcommand*{\thesis@divTAG}{#1}%
  \newcommand*{\@address}{#2}}

\newif\ifliuthesis@hyperref \liuthesis@hyperreffalse
\newcommand*\phantomsection{\relax}
\newcommand*\href[2][]{#1}
\newcommand*\pdfbookmark[3][]{\relax}


%%%%%%%%%%%%%%%%%%%%%
%%%   Language    %%%
%%%%%%%%%%%%%%%%%%%%%

\def\acknowledgementsname{Acknowledgments}%
\def\corollaryname{Corollary}
\def\definitionname{Definition}
\def\examplename{Example}
\def\lemmaname{Lemma}
\def\listexamplename{List of Examples}
\def\theoremname{Theorem}
\newcommand*{\monthName}[1]{\ifcase#1\or
  January\or February\or March\or
  April\or May\or June\or
  July\or August\or September\or
  October\or November\or December\else Unknown\fi}

\addto{\captionsenglish}{%
  \def\acknowledgementsname{Acknowledgments}%
  \def\corollaryname{Corollary}%
  \def\definitionname{Definition}%
  \def\examplename{Example}%
  \def\lemmaname{Lemma}%
  \def\listexamplename{List of Examples}%
  \def\theoremname{Theorem}%
  \renewcommand*{\monthName}[1]{\ifcase#1\or%
    January\or February\or March\or%
    April\or May\or June\or%
    July\or August\or September\or%
    October\or November\or December\else Unknown\fi}%
}
\addto{\captionsswedish}{%
  \def\acknowledgementsname{Tack}%
  \def\corollaryname{Korrolarium}%
  \def\definitionname{Definition}%
  \def\examplename{Exempel}%
  \def\lemmaname{Lemma}%
  \def\listexamplename{Exempellista}%
  \def\theoremname{Sats}%
  \renewcommand*{\monthName}[1]{\ifcase#1\or%
    januari\or februari\or mars\or%
    april\or maj\or juni\or%
    juli\or augusti\or september\or%
    oktober\or november\or december\else Unknown\fi}%
}
\DeclareOption{english}{%
  \AtBeginDocument{\selectlanguage{english}}%
}

\DeclareOption{swedish}{%
  \AtBeginDocument{\selectlanguage{swedish}}%
}

%%%%%%%%%%%%%%%%%%%%%
%%% CROP SETTINGS %%%
%%%%%%%%%%%%%%%%%%%%%

\newcommand*{\@ul}{\relax}\newcommand*{\@ur}{\relax}%
\newcommand*{\@ll}{\relax}\newcommand*{\@lr}{\relax}%
\newcommand*{\@info}{\relax}%
\DeclareOption{crop}{% To get crop marks
  \renewcommand*{\@ul}{\begin{picture}(0,0)%
      \unitlength1mm%
      \thinlines%
      \put(0,0){\line(-5,0){2}}%
      \put(0,0){\line(0,5){2}}%
    \end{picture}}%
  \renewcommand*{\@ur}{\begin{picture}(0,0)%
      \unitlength1mm%
      \thinlines%
      \put(0,0){\line(5,0){2}}%
      \put(0,0){\line(0,5){2}}%
    \end{picture}}%
  \renewcommand*{\@ll}{\begin{picture}(0,0)%
      \unitlength1mm%
      \thinlines%
      \put(0,0){\line(-5,0){2}}%
      \put(0,0){\line(0,-5){2}}%
    \end{picture}}%
  \renewcommand*{\@lr}{\begin{picture}(0,0)%
    \unitlength1mm%
    \thinlines%
    \put(0,0){\line(5,0){2}}%
    \put(0,0){\line(0,-5){2}}%
  \end{picture}}%
\PassOptionsToPackage{axes}{crop}}

\DeclareOption{info}{%
  \renewcommand*{\@info}{{%
    \global\advance\CROP@index\@ne%
    \def\x{\discretionary{}{}{\hbox{\kern.5em---\kern.5em}}}%
    \advance\paperwidth-20\p@%
    \dimen@5mm%
    \ifx\CROP@pagecolor\@empty%
    \else%
      \advance\dimen@\CROP@overlap%
    \fi%
    \hb@xt@\z@{%
      \hss%
      \vbox to\z@{%
        \centering%
        \hsize\paperwidth%
        \vss%
        \normalfont%
        \small%
        \CROP@font{%
          \texttt{\jobname}:~\the\year-\two@digits{\the\month}%
          -\two@digits{\the\day}\:\:%
          \CROP@time\x\thepage(\the\CROP@index)\strut%
        }%
        \vskip\dimen@%
      }%
      \hss
    }%
  }}%
}

\DeclareOption{noCrop}{%
  \renewcommand*{\@ul}{\relax}\renewcommand*{\@ur}{\relax}%
  \renewcommand*{\@ll}{\relax}\renewcommand*{\@lr}{\relax}%
  \renewcommand*{\@info}{\relax}%
}

%%%%%%%%%%%%%%%%%%%%%
%%%  PAGE LAYOUT  %%%
%%%%%%%%%%%%%%%%%%%%%
\newcommand*{\useHyperRef}{%
  \liuthesis@hyperreftrue%
  \let\pdfbookmark=\undefined%
  \usepackage[hyperindex, bookmarks,%
              plainpages=false, naturalnames=true]{hyperref}[2000/09/29]}
\PassOptionsToPackage{off, a4, center}{crop} % S5 centered on A4 with crops
\DeclareOption{print}{% S5 format
  \PassOptionsToPackage{off, noaxes, width=165mm, height=240mm}{crop}%
  \ExecuteOptions{noCrop}%
}

\DeclareOption{pdfRelease}{% Release in pdf format without copyright notice
  \PassOptionsToPackage{off, noaxes, a4, center}{crop}%
  \ExecuteOptions{noCrop}%
  \renewcommand*{\useHyperRef}{%
    \liuthesis@hyperreftrue%
    \let\pdfbookmark=\undefined%
    \usepackage[hyperindex, bookmarks,%
                plainpages=false, naturalnames=true]{hyperref}[2000/09/29]}
}

\DeclareOption{ePress}{% S5 centered on A4
  \AtEndDocument{\clearemptydoublepage\makeCopyright}%
  \ExecuteOptions{pdfRelease}%
}

\DeclareOption{draft}{% S5 centered on A4 DRAFT
  \PassOptionsToPackage{off, a4, center}{crop} % S5 centered on A4 with crops
  \ClassInfo{liuthesis}{This is a DRAFT mode and should hence NOT be
    used for other purposes. Pictures are blank and overful hboxes
    stand out.}
}
\AtEndOfClass{%
  \setlength{\marginparwidth}{\oddsidemargin}%
  \addtolength{\marginparwidth}{-\marginparsep}%
  \addtolength{\marginparwidth}{-\p@}%
}
\DeclareOption*{\InputIfFileExists{\CurrentOption.ltd}%
  {}{\PassOptionsToClass{\CurrentOption}{book}\OptionNotUsed}}%
\ProcessOptions\relax

\ifx\thesis@divTAG\relax
  \ClassError{liuthesis}{No division definition found.}
\else
  \ClassInfo{liuthesis}{Loading division definition \thesis@divTAG.}
\fi

\LoadClass[twoside]{book}
\newcommand*\clearemptydoublepage{\clearpage\if@twoside \ifodd\c@page\else
  \thispagestyle{empty}\hbox{}\newpage\if@twocolumn\hbox{}\newpage\fi\fi\fi}
\DeclareRobustCommand\em%
        {\@nomath\em \ifdim \fontdimen\@ne\font >\z@
                       \upshape \else \slshape \fi}
\RequirePackage{graphicx}
\RequirePackage[dvips=false, pdftex=false, vtex=false,%
  twoside, papersize={165mm,240mm}, body={125mm,195mm},%
  bindingoffset=4mm, marginparsep=3mm, portrait,%
  twocolumn=false, vmarginratio={4:5}]{geometry}[2002/07/08]

\RequirePackage{crop}[2001/11/16]
\RequirePackage[intlimits]{amsmath}
\RequirePackage{amsfonts, amssymb}
\cropdef[\@info]{\@ul}{\@ur}{\@ll}{\@lr}{liuthesiscrop}
\crop[liuthesiscrop,font=\mdseries\upshape\scriptsize]
\newcommand*{\ifLanguageInUse}[1]{%
  \expandafter\ifx\csname l@#1\endcsname\relax
  \expandafter\@secondoftwo
  \else
  \bbl@afterfi{\ifnum\csname l@#1\endcsname=\language
    \expandafter\@firstoftwo
    \else
    \expandafter\@secondoftwo
    \fi}%
  \fi}

%%%%%%%%%%%%%%%%%%%%%%%%
%%        Info        %%
%%%%%%%%%%%%%%%%%%%%%%%%

\renewcommand*{\author}[1]{\renewcommand*{\@author}{#1}%
  \ifliuthesis@hyperref\hypersetup{pdfauthor={#1}}\fi}%
\newcommand*{\thesis@day}{%
  \@latex@warning@no@line{No \noexpand\thesisDate given}}%
\newcommand*{\thesisDate}[3]{%
  \newcommand*{\thesis@month}{#1}%
  \renewcommand*{\thesis@day}{#2}%
  \newcommand*{\thesis@year}{#3}%
}
\newcommand*{\thesis@division}{%
  \@latex@warning@no@line{No \noexpand\thesisDivision given}}%
\newcommand*{\thesisDivision}[1]{\renewcommand*{\thesis@division}{#1}}%
\newcommand*{\thesis@subject}{%
  \@latex@warning@no@line{No \noexpand\subject given}}%
\newcommand*{\subject}[1]{\renewcommand*{\thesis@subject}{#1}%
  \ifliuthesis@hyperref\hypersetup{pdfsubject={#1}}\fi}%
\newcommand*{\thesis@No}{%
  \@latex@warning@no@line{No \noexpand\thesisNo given}}%
\newcommand*{\thesisNo}[1]{\renewcommand*{\thesis@No}{#1}}%
\renewcommand*{\title}[2]{\newcommand*{\swe@title}{#1}%
  \renewcommand*{\@title}{#2}%
  \ifliuthesis@hyperref%
  \ifLanguageInUse{english}{\hypersetup{pdftitle={#2}}}%
  {\hypersetup{pdftitle={#1}}}\fi}%
\newcommand*{\URL@b}{\@latex@warning@no@line{No \noexpand\URL given}}%
\newcommand*{\URL@a}{}%
\newcommand*{\URL}[2][]{%
  \renewcommand*{\URL@a}{#1}\renewcommand*{\URL@b}{#2}}%
\newcommand*{\keywords}[1]{\newcommand*{\@keywords}{#1}%
  \ifliuthesis@hyperref\hypersetup{pdfkeywords={#1}}\fi}%
\newcommand{\@abstract}{%
  \@latex@warning@no@line{No \noexpand\abstract given}}%
\newcommand{\abstract}[2][]{%
  \renewcommand{\@abstract}{\ignorespacesafterend\noindent #2\unskip}%
  \newcommand{\swe@abstract}{\ignorespacesafterend\noindent #1\unskip}}%
\newcommand{\@acknowledgements}{%
  \@latex@warning@no@line{No \noexpand\acknowledgements given}}%
\newcommand{\acknowledgements}[1]{%
  \renewcommand{\@acknowledgements}{%
    \ignorespacesafterend\noindent#1\unskip}}%
\newcommand*{\@supervisor}{%
  \@latex@warning@no@line{No \noexpand\supervisor given}}%
\newcommand*{\supervisor}[1]{%
  \renewcommand{\@supervisor}{\ignorespaces#1\ignorespaces}}%
\newcommand*{\@examiner}{%
  \@latex@warning@no@line{No \noexpand\examiner given}}%
\newcommand*{\examiner}[1]{%
  \renewcommand{\@examiner}{\ignorespaces#1\ignorespaces}}
\newcommand*{\AND}{\\\>\bfseries\ignorespaces}%
\newcommand*{\AT}{\\\>\>\small\ignorespaces}%

%%%%%%%%%%%%%%%%%%%%%%%%
%%    Library page    %%
%%%%%%%%%%%%%%%%%%%%%%%%

\newcommand{\makeLibraryPage}{%
  \cleardoublepage \thispagestyle{empty}%
  \pdfbookmark[1]{Library Page}{LibraryPage}%
  \enlargethispage*{27mm} {\centering%
  \vspace*{-11mm}
  \hspace*{3mm}%
  \makebox[\z@][l]{%
    \begin{minipage}[t][212mm]{126mm}%
      \scriptsize \setlength{\unitlength}{1mm}%
      \fontsize{7}{8\p@} \selectfont%
      \begin{picture}(126,212)(0,0)%
        %% Department and division
        \put(0,190){\makebox[\z@][l]{\framebox(92,22){}}}%
        \put(1,191){\includegraphics[width=20mm]{LiTH_sigill_sv}}%
        \put(25,209){\parbox[t]{5cm}{\textbf{Avdelning, Institution}\\%
            Division, Department}}%
        \put(28,197){\parbox[c]{64mm}{\thesis@division\\\@address}}%
        %% Date
        \put(93,190){\makebox[\z@][l]{\framebox(33,22){}}}%
        \put(94,209){\parbox[t]{5cm}{\textbf{Datum}\\%
            Date}}%      % Must be Swedish style
        \put(95,197){\thesis@year-%
          \ifnum\thesis@month<10 0\fi\thesis@month-%
          \ifnum\thesis@day<10 0\fi\thesis@day}%
        %% Language
        \put(0,157){\makebox[\z@][t]{\framebox(30,32){}}}%
        \put(1,186){\parbox[t]{3cm}{\textbf{Språk}\\Language}}%
        \put(1,178){$\Box$ Svenska/Swedish}%
        \put(1,174){$\Box$ Engelska/English}%
        \put(1,162){$\Box$} \put(5,162){\line(1,0){20}}%
        \ifLanguageInUse{english}{\put(1,174){$\boxtimes$}}{%
          \ifLanguageInUse{swedish}{\put(1,178){$\boxtimes$}}{}}%
        %% Report category
        \put(31,157){\makebox[\z@][t]{\framebox(30,32){}}}%
        \put(32,186){\parbox[t]{5cm}{\textbf{Rapporttyp}\\Report category}}%
        \put(32,178){$\Box$ Licentiatavhandling}%
        \put(32,174){$\Box$ Examensarbete}%
        \put(32,170){$\Box$ C-uppsats}%
        \put(32,166){$\Box$ D-uppsats}%
        \put(32,162){$\Box$ Övrig rapport}%
        \put(32,158){$\Box$} \put(35,158){\line(1,0){20}}%
        \put(32,174){$\boxtimes$}  % set kind of report
        %% URL
        \put(0,143){\makebox[\z@][t]{\framebox(61,13){}}}%
        \put(1,153){\textbf{URL för elektronisk version}}%
        \put(3,148){\begingroup%
          \sbox{\@tempboxa}{\mbox{\URL@a}}%
          \fontsize{5}{8\p@} \selectfont%
          \ifdim\wd\@tempboxa > \z@%
            \parbox[c]{58mm}{\href{\URL@a}{\texttt{\URL@a}}\\%
              \href{\URL@b}{\texttt{\URL@b}}}%
          \else%
            \parbox[c]{58mm}{\href{\URL@b}{\texttt{\URL@b}}}%
          \fi\endgroup}%
        %% ISBN etc
        \put(62,143){\makebox[\z@][t]{\framebox(64,46){}}}%
        \put(63,186){\textbf{ISBN}}%
        \put(63,181){\line(1,0){62}}\put(69,182){---}%
        \put(63,178){\textbf{ISRN}}%
        \put(63,171){\line(1,0){62}} \put(70,174){\thesis@No}%
        \put(63,168){\parbox[t]{5cm}{\textbf{Serietitel och serienummer}\\%
            Title of series, numbering}}%
        \put(102,168){\textbf{ISSN}}%
        \put(102,163){\line(1,0){23}} \put(102,164){---}%
        %% Title
        \put(0,108){\makebox[\z@][t]{\framebox(126,34){}}}%
        \put(1,138){\parbox[t]{5cm}{\textbf{Titel}\\Title}}%
        \put(17,138){\parbox[t]{100mm}{\swe@title\par\smallskip\@title}}%
        \put(1,117){\parbox[t]{5cm}{\textbf{Författare}\\Author}}%
        \put(17,117){\@author}%
        %% Abstract
        \put(0,12){\makebox[\z@][t]{\framebox(126,95){}}}%
        \put(1,104){\parbox[t]{5cm}{\textbf{Sammanfattning}\\Abstract}}%
        \put(17,96){\parbox[t]{100mm}{%
            \setlength{\parindent}{1.5em}\@abstract}}%
        %% Keywords
        \put(0,0){\makebox[\z@][t]{\framebox(126,11){}}}%
        \put(1,8){\parbox[t]{5cm}{\textbf{Nyckelord}\\Keywords}}%
        \put(17,5){\parbox[t]{100mm}{\@keywords}}
      \end{picture}%
    \end{minipage}%
  }}%
  \clearemptydoublepage%
}

%%%%%%%%%%%%%%%%%%%%%%%%
%%  Acknowledgements  %%
%%%%%%%%%%%%%%%%%%%%%%%%
\newcommand*{\makeAcknowledgements}{%
  \clearemptydoublepage%
  \pdfbookmark[1]{Acknowledgments}{Acknowledgments}%
  \chapter*{\acknowledgementsname}%
  \@acknowledgements%
  \clearemptydoublepage%
}

%%%%%%%%%%%%%%%%%%%%%%%%
%%      Abstract      %%
%%%%%%%%%%%%%%%%%%%%%%%%

\newcommand{\lt@twoAbstractPages}{}
\newcommand{\twoAbstractPage}{%
  \renewcommand{\lt@twoAbstractPages}{\clearemptydoublepage}}
\newcommand*{\makeAbstract}{%
  \clearemptydoublepage \thispagestyle{plain}%
  \pdfbookmark[1]{Abstract}{Abstract}%
  \begingroup
    \selectlanguage{english}
    \vspace*{-3\baselineskip}%
    \@makeschapterhead{\abstractname}  % MIGHT CHANGE!!!
    \vspace*{-1.5\baselineskip}%
    \@abstract%
  \endgroup
  \vfill \sbox{\@tempboxa}{\mbox{\swe@abstract}}%
  \ifdim\wd\@tempboxa > \z@%
    \lt@twoAbstractPages
    \begingroup
      \selectlanguage{swedish}
      \thispagestyle{plain}
      \vspace*{-3\baselineskip}%
      \@makeschapterhead{\abstractname} %MIGHT CHANGE!!!
      \vspace*{-1.5\baselineskip}%
      \swe@abstract%
    \endgroup
  \fi%
  \vfill \clearemptydoublepage%
}

%%%%%%%%%%%%%%%%%%%%%%%%
%%  Copyright notice  %%
%%%%%%%%%%%%%%%%%%%%%%%%

\newcommand*{\makeCopyright}{\begingroup%
  \cleardoublepage \thispagestyle{empty}%
    \pdfbookmark[-1]{Copyright}{Copyright}%%
  \selectlanguage{swedish}%
  \vspace*{-22mm}%
  \setlength\parskip{0\p@ \@plus \p@} \setlength{\parindent}{15\p@}%
  \hfill%
  \includegraphics[height=18mm]{LinkUniv_staende_pms}%
  %
  \vspace{3\baselineskip} \selectlanguage{swedish}%
  \par\noindent{\fontsize{12pt}{14pt}\selectfont%
    \normalfont\large\bfseries Upphovsrätt}\\[1.5ex]
  \fontsize{10pt}{12pt}\selectfont
  Detta dokument hålls tillgängligt på Internet --- eller dess
  framtida ersättare --- under 25 år från publiceringsdatum under
  förutsättning att inga extraordinära omständigheter uppstår.
  \par
  Tillgång till dokumentet innebär tillstånd för var och en att läsa,
  ladda ner, skriva ut enstaka kopior för enskilt bruk och att använda
  det oförändrat för icke\-kommersiell forskning och för undervisning.
  Överföring av upphovsrätten vid en senare tidpunkt kan inte
  upphäva detta tillstånd. All annan användning av dokumentet kräver
  upphovsmannens medgivande. För att garantera äktheten, säkerheten
  och tillgängligheten finns det lösningar av teknisk och
  administrativ art.
  \par
  Upphovsmannens ideella rätt innefattar rätt att bli nämnd som
  upphovsman i den omfattning som god sed kräver vid användning av
  dokumentet på ovan be\-skrivna sätt samt skydd mot att dokumentet
  ändras eller presenteras i sådan form eller i sådant sammanhang som
  är kränkande för upphovsmannens litterära eller konstnärliga
  anseende eller egenart.
  \par
  För ytterligare information om Linköping University Electronic Press
  se förlagets hemsida
  \href{http://www.ep.liu.se/}{\texttt{http://www.ep.liu.se/}}
  %
  \vspace*{4\baselineskip}%
  \selectlanguage{english}%
  \par\noindent{\fontsize{12pt}{14pt}\selectfont%
    \normalfont\large\bfseries Copyright}\\[1.5ex]
  \fontsize{10pt}{12pt}\selectfont
  The publishers will keep this document online on the Internet --- or
  its possible replacement --- for a period of 25 years from the date
  of publication barring exceptional circumstances.
  \par
  The online availability of the document implies a permanent permission
  for anyone to read, to download, to print out single copies for his/her
  own use and to use it unchanged for any non-commercial research and
  educational purpose. Subsequent transfers of copyright cannot revoke
  this permission. All other uses of the document are conditional on the
  consent of the copyright owner. The publisher has taken technical and
  administrative measures to assure authenticity, security and
  accessibility.
  \par
  According to intellectual property law the author has the right to
  be mentioned when his/her work is accessed as described above and to
  be protected against infringement.
  \par
  For additional information about the Linköping University Electronic
  Press and its procedures for publication and for assurance of
  document integrity, please refer to its www home page:
  \href{http://www.ep.liu.se/}{\texttt{http://www.ep.liu.se/}} \par
       {\vspace*{\baselineskip} \flushright \textcopyright\ \@author
  \clearpage} \endgroup
}

%%%%%%%%%%%%%%%%%%%%%%%%
%%    Environments    %%
%%%%%%%%%%%%%%%%%%%%%%%%

\RequirePackage{theorem}
\theorembodyfont{\normalfont\slshape}
\newtheorem{theorem}{\theoremname}[chapter]
\newtheorem{lemma}{\lemmaname}[chapter]
\newtheorem{definition}{\definitionname}[chapter]
\newtheorem{corollary}{\corollaryname}[chapter]

%%% Proof%%%
\newenvironment{proof}[1][]{
    \sbox{\@tempboxa}{\mbox{#1}}%
    \par\noindent%
    \ifdim\wd\@tempboxa > \z@%
      \textbf{\proofname~(#1)}%
    \else%
      \textbf{\proofname}%
    \fi%
    \begingroup\small%
  }%
  {%
    \endgroup%
    \hspace*{\stretch{ 1}}$\Box$\par\bigskip%
    \ignorespacesafterend%
}

%%% Example %%%
\DeclareRobustCommand*{\listofexamples}{%
  \section*{\listexamplename}%
  \@starttoc{xmp}%
}
\newcommand*{\l@example}{\l@figure}%
\newcounter{Example}[chapter]%
\renewcommand*{\theExample}{\thechapter.\arabic{Example}}%
\newlength{\examplelinewidth}
\setlength{\examplelinewidth}{.8pt}
\newenvironment{example}[1][]{%
  \refstepcounter{Example}%
  \sbox{\@tempboxa}{\ignorespaces#1\unskip}%
  \ifdim \wd\@tempboxa>\z@%
    \sbox{\@tempboxa}{\textbf{%
        \thinspace\ignorespaces%
        \examplename\unskip~\theExample:~#1\thinspace}}%
  \else%
    \sbox{\@tempboxa}{\textbf{\thinspace\examplename~\theExample\thinspace}}%
  \fi%
  \addcontentsline{xmp}{example}{\protect\numberline{\theExample}#1}%
  \@example%
}%
{\@endexample}

\newenvironment{example*}[1][]{%
  \sbox{\@tempboxa}{\ignorespaces#1\unskip}%
  \ifdim \wd\@tempboxa>\z@%
    \sbox{\@tempboxa}{\textbf{%
        \thinspace\ignorespaces\examplename:~#1\thinspace}}%
  \else%
    \sbox{\@tempboxa}{\textbf{\thinspace\examplename\thinspace}}%
  \fi%
  \@example%
}%
{\@endexample}

\newcommand*\@example{%
  \setlength{\@tempskipa}{\linewidth}%
  \addtolength{\@tempskipa}{-\wd\@tempboxa}%
  \addtolength{\@tempskipa}{-1em}%
  \pagebreak[2]%
  \strut\\[.5\baselineskip]%
  \hspace*{-\examplelinewidth}\hspace*{-.5em}%
  \rule[\examplelinewidth]{\examplelinewidth}{1ex}%
  \rule[1ex]{2em}{\examplelinewidth}%
  \raisebox{.6ex}{\box\@tempboxa}%
  \rule[1ex]{\@tempskipa}{\examplelinewidth}%
  \rule[\examplelinewidth]{\examplelinewidth}{1ex}%
  \nopagebreak[4]\hspace*{-\examplelinewidth}\hspace*{-.5em}%
  \vspace*{-1.5ex}%
  \begin{trivlist}\nopagebreak[4]\item\@afterindentfalse%
}

\newcommand*\@endexample{%
  \nopagebreak[4]\unskip\end{trivlist}\unskip%
  \nopagebreak[4]\vspace*{-1ex}%
  \hspace*{-\examplelinewidth}\hspace*{-.5em}%
  \rule{\examplelinewidth}{1ex}\rule{\linewidth}{\examplelinewidth}%
  \rule{1em}{\examplelinewidth}\rule{\examplelinewidth}{1ex}%
  \hspace*{-\examplelinewidth}\hspace*{-.5em}\\[.5\baselineskip]%
  \pagebreak[2]%
  \ignorespacesafterend%
}

%%%%%%%%%%%%%%%%%%%%%%%%
%%      Headings      %%
%%%%%%%%%%%%%%%%%%%%%%%%

\DeclareRobustCommand*{\chaptermark}[1]{\markboth{#1}{}}
\DeclareRobustCommand*{\sectionmark}[1]{\markright{\thesection~#1}{}}
\renewcommand*{\ps@headings}{%
  \renewcommand*{\@oddfoot}{}%
  \renewcommand*{\@evenfoot}{}%
  \renewcommand*{\@evenhead}{%
    \setlength{\@tempskipa}{\textwidth}%
    \addtolength{\@tempskipa}{-.25em}%
    \parbox{\@tempskipa}{\bfseries \thepage\hfill\ \ \leftmark%
      \vspace*{2\p@}\hrule}}%
  \renewcommand*{\@oddhead}{%
    \setlength{\@tempskipa}{\textwidth}%
    \addtolength{\@tempskipa}{-.25em}%
    \parbox{\@tempskipa}{\bfseries\rightmark\hfill\ \ \thepage
      \vspace*{2\p@}\hrule}%
}}

%%%%%%%%%%%%%%%%%%%%%%%%
%%    Bibliography    %%
%%%%%%%%%%%%%%%%%%%%%%%%

\renewenvironment{thebibliography}[1]{%
  \chapter*{\bibname}%
  \phantomsection\addcontentsline{toc}{chapter}{\bibname}%
  \markboth{\bibname}{\bibname}%
  \list{\@biblabel{\@arabic\c@enumiv}}%
  {\settowidth{\labelwidth}{\@biblabel{#1}}%
            \setlength{\leftmargin}{\labelwidth}%
            \addtolength{\leftmargin}{\labelsep}%
            \@openbib@code%
            \usecounter{enumiv}%
            \let\p@enumiv\@empty%
            \renewcommand\theenumiv{\@arabic\c@enumiv}}%
      \sloppy%
      \clubpenalty4000%
      \@clubpenalty \clubpenalty%
      \widowpenalty4000%
      \sfcode`\.\@m}%
{\def\@noitemerr%
  {\@latex@warning{Empty `thebibliography' environment}}%
  \endlist}%

%%%%%%%%%%%%%%%%%%%%%%%%
%%       Index        %%
%%%%%%%%%%%%%%%%%%%%%%%%

\renewenvironment{theindex}
               {\if@twocolumn
                  \@restonecolfalse
                \else
                  \@restonecoltrue
                \fi
                \columnseprule \z@
                \columnsep 35\p@
                \twocolumn[\@makeschapterhead{\indexname}]%
                \phantomsection\addcontentsline{toc}{chapter}{\indexname}%
                \markboth{\indexname}{\indexname}%
                \thispagestyle{plain}\setlength{\parindent}{\z@}%
                \setlength{\parskip}{\z@ \@plus .3\p@}%
                \let\item\@idxitem}%
               {\if@restonecol\onecolumn\else\clearpage\fi}

%%%%%%%%%%%%%%%%%%%%%%%%
%%     Sectioning     %%
%%%%%%%%%%%%%%%%%%%%%%%%

\renewcommand{\frontmatter}{%
  \thispagestyle{empty}%
  \cleardoublepage%
  \@mainmatterfalse%
  \pagenumbering{roman}}
\renewcommand{\mainmatter}{%
  \thispagestyle{empty}%
  \cleardoublepage%
  \pagestyle{headings}
  \@mainmattertrue%
  \pagenumbering{arabic}}
\renewcommand*{\backmatter}{%
  \if@openright%
    \clearemptydoublepage%
  \else%
    \clearpage%
  \fi%
  \@openrightfalse}
\newcommand{\makeFrontMatter}[1][]{%
  \makeFrontPage\thispagestyle{empty}\clearemptydoublepage%
  \frontmatter\makeTitlePage \thispagestyle{empty}\clearemptydoublepage%
  \makeLibraryPage\thispagestyle{empty}\clearemptydoublepage%
  \pagestyle{headings}\makeAbstract%
  \thispagestyle{empty}\clearemptydoublepage%
  \makeAcknowledgements\thispagestyle{empty}\clearemptydoublepage%
  \tableofcontents%
  #1%
  \clearemptydoublepage\mainmatter%
}

%%%%%%%%%%%%%%%%%%%%%%%%
%%       Other        %%
%%%%%%%%%%%%%%%%%%%%%%%%

\DeclareRobustCommand*{\tableofcontents}{%
  \chapter*{\contentsname\markboth{\contentsname}{\contentsname}}%
  \@starttoc{toc}%
}
\DeclareRobustCommand*{\listoffigures}{%
  \section*{\listfigurename}%
  \@starttoc{lof}%
}
\DeclareRobustCommand*{\listoftables}{%
  \section*{\listtablename}%
  \@starttoc{lot}%
}


\renewcommand\chapter{\if@openright\clearemptydoublepage\else\clearpage\fi
  \thispagestyle{plain}%
  \global\@topnum\z@
  \@afterindentfalse
  \secdef\@chapter\@schapter}

\renewcommand{\@makecaption}[2]{%
  \vspace{\abovecaptionskip}
  \begin{small}
    \sbox{\@tempboxa}{\textbf{#1.} #2}
    \ifdim \wd\@tempboxa > \hsize
      \textbf{#1.} #2\par
    \else
      \global \@minipagefalse
      \parbox{\hsize}{\centering \usebox{\@tempboxa}\hfil}
    \fi
  \end{small}
  \vspace{\belowcaptionskip}
}

\renewcommand{\footnoterule}{\vspace*{-3\p@}
  \noindent\rule{.5\textwidth}{.5\p@}\vspace*{2.6\p@}
}
\AtBeginDocument{\pdfbookmark[0]{Preface}{Preface}}
\newcommand*\makeFrontPage{%
  \setcounter{page}{-1}%
  \begingroup\normalfont%
  \cleardoublepage%
  \pdfbookmark[1]{Front Page}{FrontPage}%
  \thispagestyle{empty}%
  \vspace*{-17mm}\hspace*{-13mm}%
  \raisebox{\z@}[\z@][\z@]{%
    \begin{minipage}[t][230mm]{145mm}%
      \@makeFrontPage
    \end{minipage}}%
  \endgroup\clearemptydoublepage%
}
\newcommand*\makeTitlePage{%
  \cleardoublepage \thispagestyle{empty} \normalfont%
  \pdfbookmark[1]{Title Page}{TitlePage}%
  \vspace*{-17mm}\hspace*{-13mm}%
  \raisebox{\z@}[\z@][\z@]{%
    \begin{minipage}[t][220mm]{145mm}%
      \@makeTitlePage%
    \end{minipage}}%
  \clearemptydoublepage%
}






%% \CharacterTable
%%  {Upper-case    \A\B\C\D\E\F\G\H\I\J\K\L\M\N\O\P\Q\R\S\T\U\V\W\X\Y\Z
%%   Lower-case    \a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\u\v\w\x\y\z
%%   Digits        \0\1\2\3\4\5\6\7\8\9
%%   Exclamation   \!     Double quote  \"     Hash (number) \#
%%   Dollar        \$     Percent       \%     Ampersand     \&
%%   Acute accent  \'     Left paren    \(     Right paren   \)
%%   Asterisk      \*     Plus          \+     Comma         \,
%%   Minus         \-     Point         \.     Solidus       \/
%%   Colon         \:     Semicolon     \;     Less than     \<
%%   Equals        \=     Greater than  \>     Question mark \?
%%   Commercial at \@     Left bracket  \[     Backslash     \\
%%   Right bracket \]     Circumflex    \^     Underscore    \_
%%   Grave accent  \`     Left brace    \{     Vertical bar  \|
%%   Right brace   \}     Tilde         \~}
\endinput
%%
%% End of file `liuthesis.cls'.
