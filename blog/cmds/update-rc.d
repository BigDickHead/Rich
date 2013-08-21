
NAME
       update-rc.d - install and remove System-V style init script links

SYNOPSIS
       update-rc.d [-n] [-f] name remove

       update-rc.d [-n] name defaults [NN | SS KK]

       update-rc.d [-n] name start|stop NN runlevel [runlevel]...  .  start|stop NN runlevel [runlevel]...  . ...

       update-rc.d [-n] name disable|enable [ S|2|3|4|5 ]

DESCRIPTION
       update-rc.d  updates  the  System  V  style  init  script  links  /etc/rcrunlevel.d/NNname  whose target is the script
       /etc/init.d/name.  These links are run by init when it changes runlevels; they are generally used to  start  and  stop
       system  services  such as daemons.  runlevel is one of the runlevels supported by init, namely, 0123456789S, and NN is
       the two-digit sequence number that determines where in the sequence init will run the scripts.

       This manpage documents only the usage and behaviour of update-rc.d.  For a discussion  of  the  System  V  style  init
       script arrangements please see init(8) and the Debian Policy Manual.


INSTALLING INIT SCRIPT LINKS
       update-rc.d  has  two  modes  of operation for installing scripts into the boot sequence.  A legacy mode where command
       line arguments are used to decide the sequence and runlevel configuration, and the default mode where  dependency  and
       runlevel  information  in the init.d script LSB comment header is used instead.  Such header is required to be present
       in init.d scripts.  See the insserv(8) manual page for details about the  LSB  header  format.   The  boot  sequencing
       method  is  decided  during  installation  or  upgrades.   During  upgrades, if there are no loops in the dependencies
       declared by LSB headers of all installed init.d scripts and no obsolete init.d scripts, the boot system  is  converted
       to  dependency  based  boot  sequencing.  The conversion to dependency based boot sequencing is one-way.  The machines
       using the legacy mode will have a file /etc/init.d/.legacy-bootordering .

       Packages installing init.d scripts should make sure both methods work, for compatiblity with systems where the  migra‐
       tion have not been done yet.

       For legacy mode, the following section documents the old behaviour.

       When  run  with either the defaults, start, or stop options, update-rc.d makes links /etc/rcrunlevel.d/[SK]NNname that
       point to the script /etc/init.d/name.

       If any files /etc/rcrunlevel.d/[SK]??name already exist then update-rc.d does nothing.  The program was  written  this
       way  so  that it will never change an existing configuration, which may have been customized by the system administra‐
       tor.  The program will only install links if none are present, i.e., if it appears that the  service  has  never  been
       installed before.

       A  common  system  administration  error is to delete the links with the thought that this will "disable" the service,
       i.e., that this will prevent the service from being started.  However, if all links have been deleted  then  the  next
       time  the  package is upgraded, the package's postinst script will run update-rc.d again and this will reinstall links
       at their factory default locations.  The correct way to disable services is to configure the service as stopped in all
       runlevels  in  which it is started by default.  In the System V init system this means renaming the service's symbolic
       links from S to K.

       If defaults is used then update-rc.d will make links to start the service in runlevels 2345 and to stop the service in
       runlevels  016.   By  default  all  the links will have sequence number 20, but this should be overridden if there are
       dependencies. For example if daemon B depends on A, then A must be started before B and B must be killed before A. You
       accomplish  this by supplying two NN arguments. In general, core daemons should start early and be killed late, whilst
       applications can start late and be killed early. See EXAMPLES below.

       The first NN argument supplies the start sequence number and the second NN argument supplies the kill sequence number.
       Kill  scripts  are  called  first, passing a stop argument. Then start scripts are called passing a start argument. In
       either case, calls happen in ascending sequence number order.

       Supplying a single NN argument will use the same number for both start and kill links. This is supported for  backward
       compatibility  but  is  discouraged,  as it may lead to inconsistent settings. As a rule of thumb, if you increase the
       start sequence number you should also decrease the stop sequence number, and vice-versa.

       As a rule of thumb, the sequence number of the stop link should be 100 minus the sequence number of  the  start  link;
       this causes services to be stopped in the opposite order to that in which they are started.  Obviously, therefore, the
       default stop sequence number should be 80.  Defaulting to 20, as update-rc.d does, is an old bug that cannot be  fixed
       because of the risk of breaking things.

       Instead  of  defaults  one can give one or more sets of arguments specifying particular runlevels in which to start or
       stop the service.  Each of these sets of arguments starts with the keyword start or stop and  a  sequence  number  NN,
       followed  by  one  or  more runlevel numbers.  The set is terminated by a solitary full stop character.  When explicit
       specification, rather than defaults, is used there will usually be one start and one stop set.  If different  sequence
       codes  are  required in different runlevels then several start sets or several stop sets may be specified.  If this is
       done and the same runlevel is named in multiple sets then only the last one counts.  Therefore it is not  possible  to
       create multiple start or multiple stop links for a service in a single runlevel directory.

       The script /etc/init.d/name must exist before update-rc.d is run to create the links.

REMOVING SCRIPTS
       When  invoked with the remove option, update-rc.d removes any links in the /etc/rcrunlevel.d directories to the script
       /etc/init.d/name.  The script must have been deleted already.  If the script is still present then update-rc.d  aborts
       with an error message.

       update-rc.d  is usually called from a package's post-removal script when that script is given the purge argument.  Any
       files in the /etc/rcrunlevel.d directories that are not symbolic links to the script  /etc/init.d/name  will  be  left
       untouched.

DISABLING INIT SCRIPT START LINKS
       When  run  with  the  disable  [  S|2|3|4|5  ]  options,  update-rc.d  modifies existing runlevel links for the script
       /etc/init.d/name by renaming start links to stop links with a sequence number equal to the difference of 100 minus the
       original sequence number.

       When  run  with  the  enable  [  S|2|3|4|5  ]  options,  update-rc.d  modifies  existing runlevel links for the script
       /etc/init.d/name by renaming stop links to start links with a sequence number equal to the positive difference of cur‐
       rent sequence number minus 100, thus returning to the original sequence number that the script had been installed with
       before disabling it.

       Both of these options only operate on start runlevel links of S, 2, 3, 4 or 5. If no start runlevel is specified after
       the disable or enable keywords, the script will attempt to modify links in all start runlevels.


OPTIONS
       -n     Don't do anything, just show what we would do.

       -f     Force removal of symlinks even if /etc/init.d/name still exists.

EXAMPLES
       Insert links using the defaults:
          update-rc.d foobar defaults
       The equivalent dependency header would have start and stop
       dependencies on $remote_fs and $syslog, and start in
       runlevels 2-5 and stop in runlevels 0, 1 and 6.
       Equivalent command using explicit argument sets:
          update-rc.d foobar start 20 2 3 4 5 . stop 20 0 1 6 .
       More typical command using explicit argument sets:
          update-rc.d foobar start 30 2 3 4 5 . stop 70 0 1 6 .
       Insert links at default runlevels when B requires A
          update-rc.d script_for_A defaults 80 20
          update-rc.d script_for_B defaults 90 10
       Insert a link to a service that (presumably) will not be needed by any other daemon
          update-rc.d top_level_app defaults 98 02
       Insert links for a script that requires services that start/stop at sequence number 20
          update-rc.d script_depends_on_svc20 defaults 21 19
       Remove all links for a script (assuming foobar has been deleted already):
          update-rc.d foobar remove
       Example of disabling a service:
          update-rc.d -f foobar remove
          update-rc.d foobar stop 20 2 3 4 5 .
       Example of a command for installing a system initialization-and-shutdown script:
          update-rc.d foobar start 45 S . stop 31 0 6 .
       Example of a command for disabling a system initialization-and-shutdown script:
          update-rc.d -f foobar remove
          update-rc.d foobar stop 45 S .


BUGS
       See http://bugs.debian.org/sysv-rc.

FILES
       /etc/init.d/
              The directory containing the actual init scripts.

       /etc/rc?.d/
              The directories containing the links used by init and managed by update-rc.d.

       /etc/init.d/skeleton
              Model for use by writers of init.d scripts.

SEE ALSO
       Debian Policy Manual,
       /etc/init.d/skeleton,
       insserv(8), sysv-rc-conf(8), bum(8), init(8).

 vim:ft=man:
