Information for Packagers
=========================

We are already providing homemade packages for
various distros on
[OBS (Open Build System)](https://build.opensuse.org/package/show/home:alextee/zrythm#),
so feel free to use these as a base.

# Upstream URLs

Please use
https://download.savannah.nongnu.org/releases/zrythm/
to fetch tarballs. The project's home page is
https://www.zrythm.org. The git repositories are
on our [CGit instance](https://git.zrythm.org/cgit/).

# Meson

Zrythm needs a fairly recent version of Meson to build.
We don't know the exact version number but something around 45
and above should be fine. The one in Debian Stretch
will not work, but the one in stretch-backports will
work.

# Docs

See the `manpage` and `user_manual` meson options.

# Bug Reports

Bug reports and feature requests should be created
on [Savannah](https://savannah.nongnu.org/support/?group=zrythm).

# Patches

Please send a patch if something does not build
in a particular scenario and you manged to fix it, or
if you want to add compilation flags.

----

Copyright (C) 2019 Alexandros Theodotou

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.