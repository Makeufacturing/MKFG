[MKFG](../../../) / [Elevate](../../) / [User Experience](../)

# Endless Audio Experience

> A simple multi-layer audio player that combines continuous audio to provide a soundscape that situates the listener in another location. Think ocean waves, peaceful streams, secluded forests, or bustling cities.

Here's a quick project to turn a Raspberry Pi Zero, Audio Bonnet, and set of small speakers into **an endlessly looping audio player**. Just power it on and it will loop your sound file forever.

You can also **layer sounds** and even pull in realtime streams (like internet radio) if you like.



## Build / Dev Log

### Components

* Pi Zero 2 W
* 16GB+ uSD card
* USB-micro power cable
* Adafruit I2S 3W Stereo Audio Bonnet
* Adafruit 3W stereo enclosed speaker pair

### STEP 0: Pre-build -- 5 min

* Solder headers to Pi Zero 2 W if needed.
* Press on Audio Bonnet.
* Plug in speaker to Bonnet.

### STEP 1: Install OS (for headless WiFi access) -- 5 min

* Select/Download `Raspberry Pi OS Lite (64-bit, Bookworm)` (dated Nov 19, 2024, under the "other" selection of OS options) and write to 16GB+ SD card via Raspberry Pi Imager (https://www.raspberrypi.org/software/).
  * Choose "OS Customization" when beginning the flashing process...
  * General:
    * Hostname: `audiotd.local`
    * Username: `your_pi_USERNAME` Pass: `your_pi_PASSWORD`
    * Wireless LAN: `your_wifi_NAME` `your_wifi_PASSWORD` / Country `US`
    * Locale: `America/New_York` (or wherever you are)

  * Services:
    * Enable SSH. Use Password authentication.

  * Save.
  * Use settings and flash to disk!


- Eject the card, put it in the Raspberry Pi and plug in power... 
- It should get on the network (wifi) & be ready to talk (~2 minutes the first time). You may need to view your router's admin page to see what IP address it was assigned.    

### STEP 2: Login & Upgrade -- 25 min

- Make sure you're on the same network as the new board.
- Update ssh key (if needed) and log in: `ssh-keygen -R audiotd.local; ssh your_pi_USERNAME@audiotd.local`
  - `sudo apt-get update; sudo apt-get full-upgrade -y; sudo apt full-upgrade --fix-missing`


### STEP 3: Install Speaker Bonnet -- 10 min

See: https://learn.adafruit.com/adafruit-speaker-bonnet-for-raspberry-pi/raspberry-pi-usage

- `sudo apt install python3-venv; python -m venv env --system-site-packages; source env/bin/activate; sudo apt install -y wget`
- `pip3 install adafruit-python-shell`
- `wget https://github.com/adafruit/Raspberry-Pi-Installer-Scripts/raw/main/i2samp.py`
- `sudo -E env PATH=$PATH python3 i2samp.py`
  - Follow prompts. Reboot when done.
- `sudo reboot` (yes, do it again!)

### STEP 4: Install VLC for Audio Playback

* ```
  sudo apt-get install vlc -y
  ```

  * This is big and takes a while. Be patient :)

* `cvlc https://icecast.radiofrance.fr/fbbreizizel-midfi.mp3 --alsa-gain=0.05`

### STEP 5: Setup Audio Files

* Create `~/audio` dir on pi. `cd ~; mkdir audio;`

* From local computer, copy files to pi like this:
  * `scp waves_on_rocks_dinard_france_178sec.mp3 your_pi_USERNAME@audiotd.local:/home/your_pi_USERNAME/audio`
* You can find lots of great audio (like the one used in the example, thanks to **felixblume**) via [freesound.org](freesound.org).

### STEP 6: Scripting Endless Offset Playback

* For each background audio layer, we want to play 2 versions, offset by 30-60 seconds. These should also be passed through a low-pass filter such that it doesn't overly crowd the foreground stream.
* For the foreground audio stream (radio), we only want a single stream. Waiting 5-10 seconds before starting a network stream increases the chance that it will start without issue (in case wifi takes a few extra seconds to connect).
* In testing on a Raspberry Pi Zero 2 W, with 4 background audio files playing, and a single internet radio station, CPU usage is around 25% (x4, all CPUs in use), and about 25% memory (132/417MB).

* `cd ~/; nano playAudioTeleporter.sh`

  * ```bash
    #!/bin/bash
    #cvlc ~/audio/cafe_406sec_lpf.mp3 --alsa-gain=0.2 --loop --repeat&
    #cvlc ~/audio/market_894sec_lpf.mp3 --alsa-gain=0.25 --loop --repeat&
    #cvlc ~/audio/french_street_tours_167sec_lpf.mp3 --alsa-gain=0.25 --loop --repeat&
    #cvlc ~/audio/waves_kerlouan_289sec_crossfaded_lpf.mp3 --alsa-gain=0.6 --loop --repeat&
    #cvlc ~/audio/waves_finistere_2038sec.mp3 --alsa-gain=0.95 --loop --repeat&
    cvlc ~/audio/waves_on_rocks_dinard_france_178sec.mp3 --alsa-gain=0.90 --loop --repeat&
    #sleep 45;
    # cvlc https://icecast.radiofrance.fr/fbbreizizel-midfi.mp3 --loop --repeat --http-reconnect --alsa-gain=0.08 &
    #cvlc https://radiobonheur.ice.infomaniak.ch/radiobonheur-128-1.mp3 --loop --repeat --http-reconnect --alsa-gain=0.15 &
    #sleep 99;
    #cvlc ~/audio/waves_kerlouan_289sec_crossfaded_lpf.mp3 --alsa-gain=0.6 --loop --repeat&
    #cvlc ~/audio/cafe_406sec_lpf.mp3 --alsa-gain=0.2 --loop --repeat&
    #cvlc ~/audio/market_894sec_lpf.mp3 --alsa-gain=0.25  --loop --repeat&
    #cvlc ~/audio/french_street_tours_167sec_lpf.mp3 --alsa-gain=0.25 --loop --repeat&
    ```

* `chmod a+x playAudioTeleporter.sh`

* `sudo nano /etc/rc.local`

  * ```bash
    #!/bin/bash
    sudo -H -u your_pi_USERNAME /home/your_pi_USERNAME/playAudioTeleporter.sh &
    ```

* `sudo chmod a+x /etc/rc.local`

* Check status with: `sudo systemctl status rc-local.service`