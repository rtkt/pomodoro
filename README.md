# pomodoro
Simple pomodoro timer

## Compatibility
Either Windows or a decent Linux should be fine.  
It should work on OS X also but I can't port my app to it since I don't have neither hackintosh nor real mac.  
This app depends on external codecs to play an alarm file so make sure they're installed.

## Features
1. Tray integration
2. Customizability
3. Ability to zero out pomodoro count if you slack too much  
  There's an additional timer that kicks in after half of time of last break.  
  For example: if your break was 10 minutes, wait 5 more minutes right after the break and pomodoro counter will be zeroed out.  
  This feature can be disabled.
4. And an alarm sound

## Notes
If you use doesn't change PREFIX variable when building on Linux make sure you have /usr/local/share dir in $XDG_DATA_DIRS environment variable or you won't have desktop integration.

## License
GPL2

## Credits
Application icon is a part of [this icon pack](http://graphicloads.com/product/food-drink-vector-icons/)  
Fallback open/close button icons — [Dalisha icon theme](https://www.gnome-look.org/content/show.php/Dalisha?content=166286)  
Alarm — [Fresh and Clean sound theme](https://www.gnome-look.org/content/show.php/Fresh+and+Clean?content=123207&foo=5ef71a05ae89b6b3712d901cce0795fb)
