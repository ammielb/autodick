<template>
  <div class="container">
    <div class="row">
      <div>
        <label>Select Klassevlag:</label>
        <div>
          <button @click="selectNumber(1, 1)" :class="{ 'selected': selectedNumbers[0] === '1' }">1</button>
          <button @click="selectNumber(1, 2)" :class="{ 'selected': selectedNumbers[0] === '2' }">2</button>
          <span>{{ greenFlagStatus }}</span>
        </div>
      </div>
    </div>
    <div class="row">
      <div>
        <label>Select Zwarte vlag:</label>
        <div>
          <button @click="selectNumber(2, 1)" :class="{ 'selected': selectedNumbers[1] === '1' }">1</button>
          <button @click="selectNumber(2, 2)" :class="{ 'selected': selectedNumbers[1] === '2' }">2</button>
          <span>{{ redFlagStatus }}</span>
        </div>
      </div>
    </div>
    <div class="row">
      <div>
        <label>Select N vlag:</label>
        <div>
          <button @click="selectNumber(3, 1)" :class="{ 'selected': selectedNumbers[2] === '1' }">1</button>
          <button @click="selectNumber(3, 2)" :class="{ 'selected': selectedNumbers[2] === '2' }">2</button>
          <span>{{ blockedFlagStatus }}</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      selectedNumbers: ['1', '1', '1'] // Default selected numbers
    };
  },
  computed: {
    greenFlagStatus() {
      console.log('KlasseVlag status:', this.selectedNumbers[0] === '2' ? 'true' : 'false');
      return this.selectedNumbers[0] === '2' ? 'true' : 'false';
    },
    redFlagStatus() {
      console.log('Zwarte vlag status:', this.selectedNumbers[1] === '2' ? 'true' : 'false');
      return this.selectedNumbers[1] === '2' ? 'true' : 'false';
    },
    blockedFlagStatus() {
      console.log('N vlag status:', this.selectedNumbers[2] === '2' ? 'true' : 'false');
      return this.selectedNumbers[2] === '2' ? 'true' : 'false';
    }
  },
  methods: {
    selectNumber(index, number) {
      console.log('Selected number:', number);
      this.selectedNumbers[index - 1] = number.toString();
    }
  }
};
</script>

<style scoped>
.container {
  border: 2px solid black;
  padding: 20px; /* Adjust padding as needed */
}

.row {
  display: flex;
  align-items: center;
  margin-bottom: 10px;
}

button {
  border: 1px solid #ccc;
  padding: 5px 10px;
  margin-right: 5px;
  cursor: pointer;
}

button.selected {
  background-color: #007bff;
  color: white;
}

span {
  margin-left: 10px; /* Add margin for spacing between button and text */
}
</style>
