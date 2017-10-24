
Usage: ckbdpwd [-v][-h][-l][-f] <max_num> [ <layout1> [<layout2> [..]] ]
  common keyboard password:
  create a password - usable on different keyboard layouts.
  -n       do not append Line Feed '\n'
  -v       verbose output
  -h       print usage
  -l       list available keyboard layouts
  -f       test/filter mode: expect password on stdin and test filter charachters
  max_num  maximum number of hex bytes to use
  hexcode is read from standard input (stdin). Use md5sum or shasum

* application / purpose:
ckbdpwd produces passwords - with a common keyset, which are usable on different keyboard layouts.
The character set for the password is reduced - but this allows using them with YubiKey or OnlyKey
USB keys - without having to change the keyboard layout.

The password is derived reproducibly from the hexadecimal input from standard input (stdin).
You can use "echo -n 'some text' |sha1sum |ckbdpwd <options>" for password creation,
or use one of the other md5sum or sha*sum tools.

