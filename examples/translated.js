async function standalonelib () {
return 'standalone lib!'
}

function folderLib() {
return 'folderLib!'
}
standalonelib().then((e) => {
console.log(e)
})
console.log(folderLib())
