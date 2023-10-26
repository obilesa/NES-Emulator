/** @type {import('tailwindcss').Config} */
const colors = require("tailwindcss/colors");

module.exports = {
  content: ["./src/**/*.{js,jsx,ts,tsx}"],
  theme: {
    colors: {
      gray: {
        400: "#bcbcbc",
        600: "#a0a0a0",
        800: "#757575",
      },

      blue: {
        300: "#32ffff",
        400: "#37ccff",
        700: "#00153d",
      },
      black: colors.black,
      white: colors.white,
    },
    extend: {},
  },
  plugins: [],
};
