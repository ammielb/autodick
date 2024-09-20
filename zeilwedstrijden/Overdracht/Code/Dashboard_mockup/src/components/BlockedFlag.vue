<template>
    <div class="flag blocked"></div>
  </template>
  
  <script>
  export default {
    name: 'BlockedFlag'
  };
  </script>
  
  <style scoped>
  .flag {
    width: 20px;
    height: 20px;
    border-radius: 50%;
  }
  
  .blocked {
    background-color: gray;
  }
  </style>
  