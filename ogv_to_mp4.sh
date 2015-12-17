ffmpeg -i demo.ogv        -c:v libx264 -preset veryslow -crf 22        -c:a libmp3lame -qscale:a 2 -ac 2 -ar 44100        demo.mp4
