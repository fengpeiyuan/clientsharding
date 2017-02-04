#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "rbtree.h"
#include "murmurhash.h"

#define LIB_VERSION "0.0.1"
#define LIB_NAME 	"clientsharding"
#define NODE_NUM 	160

static int init(lua_State *L);
static int close(lua_State *L);
static uint64_t hash(const char *hash_str,size_t len);
static int position(lua_State *L);

/*
	init({{location="redis_6379"},{location="redis_6380"},{location="redis_6381"}})
*/
static int init(lua_State *L){
	RBTree *rbtree = NULL;	
	/*验证参数*/
	luaL_checktype(L,1,LUA_TTABLE);
	/*把一个 nil 压栈*/
	lua_pushnil(L);/*－1 --> nil,-2 -->table*/

	/*
	 *从栈上弹出一个 key（键）， 然后把索引指定的表中 key-value（健值）对压入堆栈 （指定 key 后面的下一 (next) 对）。 
	 *如果表中以无更多元素， 那么 lua_next 将返回 0 （什么也不压入堆栈）
	 */
	while(0 != lua_next(L,-2)){
		const 	char *location;
		const 	char *name;
		size_t 	weight;
		size_t location_len;
		/*当前值栈为:-1 -->value,-2 --> key,index-->table
		 *把堆栈上给定有效处索引处的元素作一个拷贝压栈
		 */
		lua_pushvalue(L, -2);
		/*前值栈为:-1 -->key,-2 -->value,-3 --> key,index-->table*/

		/*验证参数*/
		luaL_checktype( L, -1, LUA_TNUMBER );
		luaL_checktype( L, -2, LUA_TTABLE );
		/*获取table索引值*/
		size_t index = lua_tointeger(L, -1);
		/*void lua_getfield (lua_State *L, int index, const char *k);
		 *把 t[k] 值压入堆栈， 这里的 t 是指有效索引 index 指向的值
		 */
		lua_getfield(L,-2,"location");
		/*当前值栈为: -1--> table['location'] ,-2 -->key,-3 -->value,-4--> key,index-->table*/
		location=lua_tolstring(L,-1,&location_len);
		if(NULL == location || 0 == location_len){
			const char *errorMessage=lua_pushfstring(L,"location must not be empty at %d in table.",index);	
			luaL_argerror( L ,2,errorMessage);
			return 0;
		}
		lua_pop(L, 1);

		lua_getfield(L,-2,"name");
		name = lua_tostring(L,-1);
		lua_pop(L,1);

		lua_getfield(L,-2,"weight");
		weight = lua_tointeger(L,-1);
		weight = (0==weight) ? 1 : weight;
		lua_pop(L,1);

		char hash_str[100];
		if(NULL == name){
			for (int n = 0; n < NODE_NUM * weight; ++n){	
				size_t len=snprintf(hash_str,100,"SHARD-%zu-NODE-%d", (index-1), n );
				int64_t  hashCode=(int64_t)hash(hash_str,len);
				insertTreeNode(&rbtree,hashCode,location);
			}
		}else{
			for (int n = 0; n < NODE_NUM * weight; ++n){
				size_t len=snprintf(hash_str,100,"%s*%zu%d",name,weight,n);
				int64_t  hashCode=(int64_t)hash(hash_str,len);
				insertTreeNode(&rbtree,hashCode,location);
			}
		}
		lua_pop(L, 2);
	}

	void **userdata = lua_newuserdata(L,sizeof(void *));
	*userdata=rbtree;

	/*Create GC method to clean up rbtree*/
	lua_newtable(L);
	lua_pushcfunction( L , close);
	lua_setfield( L , -2 , "__gc");
	lua_setmetatable( L , -2);

	lua_pushcclosure(L, position, 1);
	return 1;
}

/*
	quick_init({"redis_6379","redis_6380","redis_6381"})
*/
static int quick_init(lua_State *L){
	RBTree *rbtree = NULL;	
	/*验证参数*/
	luaL_checktype(L,1,LUA_TTABLE);
	/*把一个 nil 压栈*/
	lua_pushnil(L);/*－1 --> nil,-2 -->table*/
	
	/*
	 *从栈上弹出一个 key（键）， 然后把索引指定的表中 key-value（健值）对压入堆栈 （指定 key 后面的下一 (next) 对）。 
	 *如果表中以无更多元素， 那么 lua_next 将返回 0 （什么也不压入堆栈）
	 */
	while(0 != lua_next(L,-2)){
		const 	char *location;
		size_t 	weight = 1;
		size_t location_len;
		/*
		 *当前值栈为:-1 -->value,-2 --> key,index-->table
		 *把堆栈上给定有效处索引处的元素作一个拷贝压栈
		 */
		lua_pushvalue(L, -2);
		/*前值栈为:-1 -->key,-2 -->value,-3 --> key,index-->table*/

		/*验证参数*/
		luaL_checktype( L, -1, LUA_TNUMBER );
		luaL_checktype( L, -2, LUA_TSTRING );
		/*获取table索引值*/
		size_t index = lua_tointeger(L, -1);
		location = lua_tolstring(L,-2,&location_len);
		if(NULL == location || 0 == location_len){
			const char *errorMessage=lua_pushfstring(L,"location must not be empty at %d in table.",index);	
			luaL_argerror( L ,2,errorMessage);
			return 0;
		}

		char hash_str[100];
		for (int n = 0; n < NODE_NUM * weight; ++n){	
			size_t len = snprintf(hash_str,100,"SHARD-%zu-NODE-%d", (index-1), n );
			int64_t  hashCode = (int64_t)hash(hash_str,len);
			insertTreeNode(&rbtree,hashCode,location);
		}
		lua_pop(L, 2);
	}

	void **userdata = lua_newuserdata(L,sizeof(void *));
	*userdata = rbtree;

	/*Create GC method to clean up rbtree*/
	lua_newtable(L);
	lua_pushcfunction( L , close);
	lua_setfield( L , -2 , "__gc");
	lua_setmetatable( L , -2);

	lua_pushcclosure(L, position, 1);
	return 1;
}

static int position(lua_State *L){
	size_t len;
	const char  *key;
	luaL_argcheck(L, lua_gettop(L) == 1, 1, "expected 1 argument");
	key=luaL_checklstring(L,1,&len);
	lua_pushvalue(L, lua_upvalueindex(1));
    	void **userdata=lua_touserdata(L, -1);
    	if (!userdata)
        	luaL_error(L, "ERROR: Unable to use other method before init");
    	RBTree *rbtree = *userdata;
	RBTreeNode *rbTreeNode = tailTreeNode(&rbtree,hash(key,len));
	if(NULL == rbTreeNode){
		rbTreeNode = minTreeNode(&rbtree);
	}
	lua_pushstring(L,rbTreeNode->data);
	return 1;
}

static int close(lua_State *L){
    	void **userdata = lua_touserdata(L, 1);
	RBTree *rbtree=*userdata;
	if(rbtree){
		destory(&rbtree);
	}	
	rbtree = NULL;
	printf("destory success!\n" );
	return 0;
}

static uint64_t hash(const char *hash_str,size_t len){
	uint64_t seed=0x1234ABCD;
	uint64_t hashCode=murmurHash64A(hash_str,len,seed);
	return hashCode ? hashCode : 1;
}

int luaopen_clientsharding(lua_State *L){
	const luaL_Reg methods[]={
		{"init",init},
		{"quick_init",quick_init},
		{NULL,NULL}
	};
	luaL_register(L,LIB_NAME,methods);
	lua_pushlightuserdata(L , NULL);
	lua_setfield(L , -2,"null");
	lua_pushliteral(L,LIB_VERSION);
	lua_setfield(L, -2 ,"version");
	printf("init clientsharding ...\n");
	return 1;
}
