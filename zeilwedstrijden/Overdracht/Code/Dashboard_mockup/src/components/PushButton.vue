<template>
    <button @click="handleClick" class="push-button">{{ text }}</button>
  </template>
  
  <script>
  export default {
    props: {
      text: String
    },
    methods: {
      handleClick() {
        this.$emit('click');
      }
    }
  };
  </script>
  
  <style scoped>
  .push-button {
    background-color: #007bff;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 16px;
  }
  
  .push-button:hover {
    background-color: #0056b3;
  }
  </style>
  