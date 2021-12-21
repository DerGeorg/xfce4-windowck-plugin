# xfce4-windowck-plugin
Xfce panel plugin which allows to put the maximized window title and windows buttons on the panel.

## Whats new in this Fork?
* In "*Only Maximized Windows*" Mode, the buttons disappear if a non maximized Window is getting in focus
* New Icon Pack![minimize](https://user-images.githubusercontent.com/22521386/145676169-79b267c9-b882-4eaf-81b7-ef4ee42efb13.png)
![minimize_focused_pressed](https://user-images.githubusercontent.com/22521386/145676170-13caf8f4-eef2-44ea-a409-80a6eb4675d0.png)
![maximize_focused_prelight](https://user-images.githubusercontent.com/22521386/145676173-c0a4e600-518c-4c76-876f-17a938c984ab.png)
![maximize](https://user-images.githubusercontent.com/22521386/145676174-5cdd0655-81f3-4844-9ad6-f68d0e6f8cb3.png)
![close_focused_prelight](https://user-images.githubusercontent.com/22521386/145676175-38d0a02b-65ff-46b6-a2a6-c5e29e1555de.png)
![close](https://user-images.githubusercontent.com/22521386/145676177-f3ff5868-b230-4710-a166-9c630d22ed78.png)
![unfocused](https://user-images.githubusercontent.com/22521386/145676179-f6d52120-8f28-4fd2-a76b-a6d174b63c58.png)

## Whats up next?
* Only show Windowck Buttons on right Screen, where the maximized window is located
  * Notes:
    * Check active screen
    * Before hiding buttons, check if old active maximized window is on other screen
      * If on other screen --> do not hide buttons
      * If on same screen and maximized --> change buttons to new window
      * If on same screen and not maximized --> leave old window control

## Original plugin's features:

* Show the title and buttons of the maximized window on the panel.
* Allow window actions on buttons and title clicks (activate, (un)maximize, close).
* Allow window action menu on left button click.
* Title formatting options.
* xfwm4/unity theming support for buttons.

FAQ:  
Q: How do I hide the window decorations on maximized windows on Xfce?  
A: Xfce 4.12 now support Titileless feature!! Enable it in window manager tweaks → accessibility.

This code is derived from original 'Window Applets' from Andrej Belcijan.
See http://gnome-look.org/content/show.php?content=103732 for infos.

# Screenshots
![maximized_not_active](https://user-images.githubusercontent.com/22521386/145676271-f5527156-7889-4bd9-9296-568db3f38e74.png)
![maximized_active_window](https://user-images.githubusercontent.com/22521386/145676272-17925c9c-b84a-4a38-b484-35daa355b3d3.png)


# Installation

#### Other Linux distributions

* Install dependencies:
  - For debian/ubuntu see debian/control folder and follow debian packaging guidlines
* Generate common makefiles:
  ```
  ./autogen.sh --prefix=/usr
  ```
* compile and install the plugin
  ```bash
  make
  sudo make install
  ```
