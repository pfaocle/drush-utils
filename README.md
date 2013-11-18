# Drush Utilities

A group of Drush utility commands.


## Find Callback

Scans all enabled modules that declare `hook_menu()` implementations for an exact or partial path, passed to the command as an argument.

Usage:

    drush find-callback views/ajax

    drush find-callback views/ajax --match=partial


## Page Timer

**[Does something]** a specified number of times and reports the average page execution time.

Usage:

    drush page-timer
    drush page-timer --show-all
    drush page-timer --show-all --count=50
    drush page-timer --count=50
