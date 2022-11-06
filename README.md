What is Caa?
------------
**Caa** is (or should be - in future) a simple audio path analyzer focusing on live audio productions. 

At the moment Caa is able to
- calculate the impulse response from input and measurement signal
- calculate magnitude response from this impulse response
- calculate phase response from this impulse reponse

Furthermore, Caa can
- identify multiple systems in parallel (number by configuration parameter in the source code)
- do exponential averaging over time
- do smoothing in the frequency domain
- do windowing of the impulse response 
- automatically identify the system delay (not sure how good it works in different circumstances )

Caa is using Jack Audio Connection Kit as audio engine. 
At the moment the software is meant for user with a bit of technical background. Maybe there will be an "easy" mode in future with useful default configurations. 

License
-------
Caa is licensed under the terms
of the GNU General Public License, version 3. A copy of the license
can be found in the file LICENSE which can be found in the source
archive. You can read it here: http://www.gnu.org/licenses/gpl-3.0.html

How To Build
------------
Caa uses the QT framework and build environment. Simply type
> qmake

> make



