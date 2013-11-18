# Drush Utilities

A group of Drush utility commands.


## Array Dump

Based on [an idea by clemens](http://build2be.com/content/can-you-dig-it-drupal-nested-array), this command outputs a flattened array returned by a Drupal core or contrib function as a list of long-key:value pairs. This can be grep'd for advanced array poking.

Usage:

    drush array-dump token_info
    drush array-dump token_info | grep "^tokens\.user"


## Find Callback

Scans all enabled modules that declare hook_menu() implementations for an exact or partial path, passed to the command as an argument.

Usage:

    drush find-callback views/ajax
    drush find-callback views/ajax --match=partial
