<template>
  <div id="app">
    <div class="content">
      <div>
        <div id="states">
          <GuiScreen/>
          <HornVisualizer ref="horn"/>
        </div>
        <LogVisualizer ref="console"/>
      </div>  
      <div class="button-container">
        <NumberSelector/>
        <PushButton text="Race Afbreken" @mousedown="handleButtonClick(1)"/>
        <PushButton text="Uitstel" @mousedown="handleButtonClick(2)"/>
        <PushButton text="Toeter" @mousedown="holdHonk()" @mouseup="handleButtonClick(3)"/>
        <PushButton text="Finish" @mousedown="handleButtonClick(4)"/>
        <PushButton text="Start / Stop" @mousedown="handleButtonClick(5)"/>
      </div>
    </div>
  </div>
</template>

<script>
import GuiScreen from './components/ScreenSaver'
import PushButton from './components/PushButton'
import NumberSelector from './components/NumberSelector'
import HornVisualizer from './components/HornVisualizer.vue'
import LogVisualizer from './components/LogVisualizer.vue'

export default {
  name: 'App',

  components: {
    GuiScreen,
    PushButton,
    NumberSelector,
    HornVisualizer,
    LogVisualizer
  },
  methods: {
    handleButtonClick(buttonNumber) {
      const horn = this.$refs.horn;
      const console = this.$refs.console;
      switch (buttonNumber) {
        case 1:
          alert('De race word afgebroken, er gaan drie toeters achterelkaar af en nog een toeter één minuut vooor het waarschuwingssignaal.');
          console.log("Race afbreken, N-vlag gehesen");
          horn.requestHonk(3);
          horn.queuHonk(5000);
          break;
        case 2:
          alert('De race word uitgesteld, er gaan twee toeters achterelkaar af en nog een toeter één minuut vooor het waarschuwingssignaal.');
          console.log("Uitstel voor de start, stop procedure");
          horn.requestHonk(2);
          horn.queuHonk(5000);
          break;
        case 3:
          console.log("Toeter losgelaten");
          horn.honk(0);
          break;
        case 4:
          alert('De race is afgelopen.');
          console.log("Finish of boironding");
          break;
        case 5:
          alert('Start / Stop');
          console.log("Procedure gestart / gestopt");
          console.log("Nog 3 minuten voordat race begint, hijs klassevlag");
          horn.queuHonk(2000);

          setTimeout(() => {
            console.log("Nog 2 minuten voordat race begint, hijs zwart");
            horn.queuHonk(2000);
          }, 60000);

          setTimeout(() => {
            console.log("Nog 1 minuut voordat race begint, zwart omlaag");
            horn.queuHonk(2000);
          }, 120000);
          
          setTimeout(() => {
            console.log("Start race, klassevlag omlaag");
            horn.queuHonk(2000);
          }, 180000);
          break;
        default:
          break;
      }
    },

    holdHonk(){
      const console = this.$refs.console;
      const horn = this.$refs.horn;
      console.log("Toeter ingedrukt");
      horn.honk(1);
    }
  }
}
</script>

<style>
#app {
  display: flex;
  justify-content: center;
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}

#states {
  display: flex;
}

.content {
  display: grid;
  grid-template-columns: 1fr 1fr;
}

.button-container {
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  margin-left: 300px;
}

.button-container > * {
  margin-bottom: 10px;
}
</style>
