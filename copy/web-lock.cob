      *
      *
      *
      *
       01 lc-web-lock.
          02 lc-lock-file   pic x(80).
          02 filler         pic x value low-value.
          02 lc-lock-mode   pic 9(10) binary.
            88 lc-lock-exclusive value 1.
            88 lc-lock-share     value 0.
          02 lc-id          pic 9(10) binary.

