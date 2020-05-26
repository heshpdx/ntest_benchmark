# NTest
NTest is a strong othello program.

This repository is solely intended for integrating NTest into a benchmark suite. For any other purpose 
we recommend looking at the original NTest code.

# Licensing

NTest is licensed under the GNU General Public License, version 3. A copy of the license is included in the
GPLv3.txt file.

# Building and quickly running a benchmark

The preferred way of building ntest is through akro build, a C++ build system that I (Vlad Petric) developed.

You should be able to easily install it if you already have a recent Ruby (>= 2.2)  as follows:

``gem install akro``

Then:

``akro release/speed_test.exe``

``./release/speed_test.exe``

