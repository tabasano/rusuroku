# rusuroku

record by threshold.
in current version, recording on/off is switched by checking buffer each one second.


usage:

```
 > rusuroku.exe
```

initialize with rusuroku.ini in the same folder as executed.
set wave format in it.
threshold value must be between 0.0 and 1.0 in any int/float mode.
when it is set to 0.0, all sound are recorded.
the other hand, when it is set to 1.0, nothing is recorded.
waiting for new returning recorded buffer, drawing wave picture delays 3 seconds.
wait sometime for checking real effect by live setting threshold.



around source, cf.
[1](http://eternalwindows.jp/winmm/wave/wave04.html)
[2](http://www.fftw.org/install/windows.html)
[3](http://www.koj-m.sakura.ne.jp/hurobint/index.php?fftw)

---------------


## License
MIT
