mm
==

> movie manager

Browse tv shows on a remote machine and stream them right into `mpv`.
No server-side application, no Web UI.

Folder structure
----------------

The expected folder structure is as follows

```
Shows directory
|
|- Show 1
|   |- Season 1
|   |   |- Episode 1
|   |   |- ...
|   |   |- Episode 10
|   |- Season 2
|       |- Episode 1
|       |- ...
|       |- Episode 10
|- Show 2
    |- Season 1
        |- Episode 1
        |- ...
        |- Episode 10
```

Usage
-----

``` sh
mm domain user directory
```

Where `domain` is the domain of some machine, `user` is your user on that machine (authorization is expected to happen using a keyfile) and `directory` is the absolute path to tv shows directory on the machine (no trailing `/`).

Build
-----

``` sh
make all
```

Depends on `libssh` and `curses`.
