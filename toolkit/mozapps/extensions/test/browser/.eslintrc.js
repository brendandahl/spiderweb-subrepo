"use strict";

module.exports = { // eslint-disable-line no-undef
  "extends": [
    "../../../../../testing/mochitest/browser.eslintrc.js"
  ],

  "rules": {
    "no-unused-vars": ["error", {"args": "none", "varsIgnorePattern": "^(Cc|Ci|Cr|Cu|EXPORTED_SYMBOLS|end_test)$"}],
  }
};
