This program converts NIST SRE speech files to speech/non-speech segmentation
file in TIMIT .phn format.

Example:


For mic speech:

../bin/sph2phn -sph eslnc.sph -phn eslnc_A.phn -ch A -dn Y -df eslnc_A.wav -af 0.95
../bin/sph2phn -sph ftvhv.sph -phn ftvhv_A.phn -ch A -dn Y -df ftvhv_A.wav -af 0.95

If you find that the denoise wave file contains pulses, you may set -fs to 1024.

For clean tel speech, you may disable denoising (-dn N) and set -af to 0.99.

For technical details, visit the SSVAD site in my homepage and download the papers of SSVAD:
http://bioinfo.eie.polyu.edu.hk/ssvad/ssvad.htm


Man-Wai MAK
